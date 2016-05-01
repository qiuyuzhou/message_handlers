//
// Created by 邱宇舟 on 16/5/1.
//

#include "gtest/gtest.h"
#include "../message_handlers.h"
#include "foo.pb.h"


TEST(BASIC, Enqueue){
    message_handlers<void*, message_handlers_array_policy<void*, 256> >    ph;
    ph.add_handler<Person>(0, [](int id, Person& person, void* context){
//        cout << person.name() << endl
//        << person.id() << endl
//        << person.email() << endl;
    });

    Person person;
    person.set_name("John Doe");
    person.set_id(1234);
    person.set_email("jdoe@example.com");

    auto buf = person.SerializeAsString();
    ASSERT_NO_THROW(ph.process(0, buf.data(), buf.length(), nullptr));
}


