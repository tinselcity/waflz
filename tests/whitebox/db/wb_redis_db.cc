//: ----------------------------------------------------------------------------
//: Copyright (C) 2016 Verizon.  All Rights Reserved.
//: All Rights Reserved
//:
//: \file:    TODO.cc
//: \details: TODO
//: \author:  Reed P. Morrison
//: \date:    12/06/2016
//:
//:   Licensed under the Apache License, Version 2.0 (the "License");
//:   you may not use this file except in compliance with the License.
//:   You may obtain a copy of the License at
//:
//:       http://www.apache.org/licenses/LICENSE-2.0
//:
//:   Unless required by applicable law or agreed to in writing, software
//:   distributed under the License is distributed on an "AS IS" BASIS,
//:   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//:   See the License for the specific language governing permissions and
//:   limitations under the License.
//:
//: ----------------------------------------------------------------------------
//: ----------------------------------------------------------------------------
//: includes
//: ----------------------------------------------------------------------------
#include "catch/catch.hpp"
#include "waflz/def.h"
#include "waflz/db/redis_db.h"
#include "support/time_util.h"
#include "support/ndebug.h"
#include <unistd.h>
#include <string.h>
//: ----------------------------------------------------------------------------
//: constants
//: ----------------------------------------------------------------------------
#define MONKEY_KEY "TEST::KEY::MONKEY::BONGO"
#define BANANA_KEY "TEST::KEY::BANANA::SMELLY"
//: ----------------------------------------------------------------------------
//: kycb_db
//: ----------------------------------------------------------------------------
TEST_CASE( "redis db test", "[redis]" ) {
        SECTION("validate bad init") {
                ns_waflz::redis_db l_db;
                REQUIRE((l_db.get_init() == false));
                const char l_bad_host[] = "128.0.0.1";
                int32_t l_s;
                l_s = l_db.set_opt(ns_waflz::redis_db::OPT_REDIS_HOST, l_bad_host, strlen(l_bad_host));
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                l_s = l_db.init();
                REQUIRE((l_s == WAFLZ_STATUS_ERROR));
                //printf("error: %s\n", l_db.get_err_msg());
        }
        SECTION("validate good init") {
                ns_waflz::redis_db l_db;
                REQUIRE((l_db.get_init() == false));
                const char l_host[] = "127.0.0.1";
                int32_t l_s;
                l_s = l_db.set_opt(ns_waflz::redis_db::OPT_REDIS_HOST, l_host, strlen(l_host));
                l_s = l_db.set_opt(ns_waflz::redis_db::OPT_REDIS_PORT, NULL, 6379);
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                l_s = l_db.init();
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                int64_t l_val;
                l_s = l_db.increment_key(l_val, MONKEY_KEY, 1000);
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                //NDBG_PRINT("l_val: %ld\n", l_val);
                REQUIRE((l_val == 1));
                l_s = l_db.increment_key(l_val, MONKEY_KEY, 1000);
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                //NDBG_PRINT("l_val: %ld\n", l_val);
                REQUIRE((l_val == 2));
                l_s = l_db.increment_key(l_val, BANANA_KEY, 1000);
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                //NDBG_PRINT("l_val: %ld\n", l_val);
                REQUIRE((l_val == 1));
                l_s = l_db.increment_key(l_val, BANANA_KEY, 1000);
                l_s = l_db.increment_key(l_val, BANANA_KEY, 1000);
                //NDBG_PRINT("PRINT ALL KEYS\n");
                //l_db.print_all_keys();
                l_s = l_db.get_key(l_val, MONKEY_KEY, strlen(MONKEY_KEY));
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                //NDBG_PRINT("l_val: %ld\n", l_val);
                REQUIRE((l_val == 2));
                l_s = l_db.increment_key(l_val, MONKEY_KEY, 1000);
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                //printf("l_val: %ld\n", l_val);
                REQUIRE((l_val == 3));
                l_s = l_db.increment_key(l_val, BANANA_KEY, 1000);
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                //NDBG_PRINT("l_val: %ld\n", l_val);
                REQUIRE((l_val == 4));
                //sprintf("error: %s\n", l_db.get_err_msg());
                // wait for monkey key to expire
                sleep(1);
                l_s = l_db.get_key(l_val, MONKEY_KEY, strlen(MONKEY_KEY));
                REQUIRE((l_s == WAFLZ_STATUS_ERROR));
                l_s = l_db.increment_key(l_val, MONKEY_KEY, 1000);
                REQUIRE((l_s == WAFLZ_STATUS_OK));
                //NDBG_PRINT("l_val: %ld\n", l_val);
                REQUIRE((l_val == 1));
        }
}
