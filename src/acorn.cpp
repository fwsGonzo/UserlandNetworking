// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015-2016 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <os>
#include <acorn>

using namespace std;
using namespace acorn;

using UserBucket     = bucket::Bucket<User>;
using SquirrelBucket = bucket::Bucket<Squirrel>;

std::shared_ptr<UserBucket>     users;
std::shared_ptr<SquirrelBucket> squirrels;

std::unique_ptr<mana::Server> server_;
std::unique_ptr<mana::dashboard::Dashboard> dashboard_;

Disk_ptr disk;

#include <isotime>
#include <net/inet4>

void acorn_start()
{
  disk = fs::shared_memdisk();

  // init the first legit partition/filesystem
  disk->init_fs(
  [] (fs::error_t err, auto& fs)
  {
      if (err) panic("Could not mount filesystem...\n");

      /** IP STACK SETUP **/
      // Bring up IPv4 stack on network interface 0
      auto& stack = net::Inet4::ifconfig(5.0,
        [] (bool timeout) {
          printf("DHCP resolution %s\n", timeout ? "failed" : "succeeded");
          if (timeout)
          {
            /**
             * Default Manual config. Can only be done after timeout to work
             * with DHCP offers going to unicast IP (e.g. in GCE)
             **/
            net::Inet4::stack().network_config({ 10,0,0,42 },     // IP
                                               { 255,255,255,0 }, // Netmask
                                               { 10,0,0,1 },      // Gateway
                                               { 8,8,8,8 });      // DNS
          }
        });

      /** BUCKET SETUP */

      // create squirrel bucket
      squirrels = std::make_shared<SquirrelBucket>(10);
      // set member name to be unique
      squirrels->add_index<std::string>("name",
      [](const Squirrel& s)->const auto&
      {
        return s.get_name();
      }, SquirrelBucket::UNIQUE);

      // seed squirrels
      squirrels->spawn("Alfred"s,  1000U, "Wizard"s);
      squirrels->spawn("Alf"s,     6U,    "Script Kiddie"s);
      squirrels->spawn("Andreas"s, 28U,   "Code Monkey"s);
      squirrels->spawn("AnnikaH"s, 20U,   "Fairy"s);
      squirrels->spawn("Ingve"s,   24U,   "Integration Master"s);
      squirrels->spawn("Martin"s,  16U,   "Build Master"s);
      squirrels->spawn("Rico"s,    28U,   "Mad Scientist"s);

      // setup users bucket
      users = std::make_shared<UserBucket>();
      users->spawn();
      users->spawn();

      /** ROUTES SETUP **/
      using namespace mana;
      Router router;

      // setup Squirrel routes
      router.use("/api/squirrels", routes::Squirrels{squirrels});
      // setup User routes
      router.use("/api/users", routes::Users{users});
      // setup Language routes
      router.use("/api/languages", routes::Languages{});


      /** DASHBOARD SETUP **/
      dashboard_ = std::make_unique<dashboard::Dashboard>(8192);
      // Add singleton component
      dashboard_->add(dashboard::Memmap::instance());
      //dashboard_->add(dashboard::StackSampler::instance());
      dashboard_->add(dashboard::Status::instance());
      // Construct component
      dashboard_->construct<dashboard::Statman>(Statman::get());
      dashboard_->construct<dashboard::TCP>(stack.tcp());
      //dashboard_->construct<dashboard::CPUsage>(500ms);
      //dashboard_->construct<dashboard::Logger>(*logger_, static_cast<size_t>(50));

      // Add Dashboard routes to "/api/dashboard"
      router.use("/api/dashboard", dashboard_->router());

      // Fallback route for angular application - serve index.html if route is not found
      router.on_get("/app/.*",
      [&fs](auto, auto res) {
        #ifdef VERBOSE_WEBSERVER
        printf("[@GET:/app/*] Fallback route - try to serve index.html\n");
        #endif
        fs.cstat("/public/app/index.html", [res](auto err, const auto& entry) {
          if(err) {
            res->send_code(http::Not_Found);
          } else {
            // Serve index.html
            #ifdef VERBOSE_WEBSERVER
            printf("[@GET:/app/*] (Fallback) Responding with index.html. \n");
            #endif
            res->send_file({disk, entry});
          }
        });
      });
      INFO("Router", "Registered routes:\n%s", router.to_string().c_str());


      /** SERVER SETUP **/
      server_ = std::make_unique<Server>(stack.tcp());
      // set routes and start listening
      server_->set_routes(router).listen(80);


      /** MIDDLEWARE SETUP **/
      // custom middleware to serve static files
      auto opt = {"index.html"};
      Middleware_ptr butler = std::make_shared<middleware::Butler>(disk, "/public", opt);
      server_->use(butler);

      // custom middleware to serve a webpage for a directory
      Middleware_ptr director = std::make_shared<middleware::Director>(disk, "/public/static");
      server_->use("/static", director);

      Middleware_ptr parsley = std::make_shared<middleware::Parsley>();
      server_->use(parsley);

      Middleware_ptr cookie_parser = std::make_shared<middleware::Cookie_parser>();
      server_->use(cookie_parser);

    }); // < disk

}
