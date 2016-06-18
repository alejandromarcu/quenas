#include <stdexcept> 
#include <iostream>

using namespace std;

namespace simulator {
    void runTests();

    namespace address {
        void runTests();
    }
    namespace dataUnit {
        void runTests();
    }
    namespace layer {
        void runTests();
    }
    namespace command {
        void runTests();
    }
    namespace notification {
        void runTests();
    }

    namespace hypercube {
        namespace dataUnit {
            void runTests();
        }

    }    

/**
 * @brief Run all the tests for the simulator
 */
void runAllTests() {
    try {
        simulator::address::runTests();
        simulator::dataUnit::runTests();
        simulator::layer::runTests();
        simulator::command::runTests();    
        simulator::notification::runTests();        
        simulator::hypercube::dataUnit::runTests();
        simulator::runTests();      
        
    } catch (exception& e) {
        cout << "Exception: " << e.what() << endl;
    } catch (...) {
        cout << "Unknown Exception" << endl;
}
}

}
