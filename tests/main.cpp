#include <iostream>

using namespace std;

#include "foo.pb.h"
#include "../message_handlers.h"

int main() {

    message_handlers<policy::array_policy<256> >    ph;
    ph.add_handler<Person>(0, [](int id, Person& person){
        cout << person.name() << endl
        << person.id() << endl
        << person.email() << endl;
    });

    Person person;
    person.set_name("John Doe");
    person.set_id(1234);
    person.set_email("jdoe@example.com");

    auto buf = person.SerializeAsString();
    ph.process(0, buf.data(), buf.length());

    return 0;
}