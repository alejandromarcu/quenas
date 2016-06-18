#include "UnitTest.h"
#include "Notification.h"
#include "TypeFilter.h"

namespace simulator {
    namespace notification {
      
namespace tests {


/**
 * Test the TypeFilter class
 */
void testTypeFilter() 
{
    UnitTest u("testTypeFilter");

    TypeFilter f;
    u.isFalse(f.isAccepted("anyType"), "by default it should not accept anything");
    
    f.accept("accept");
    u.isFalse(f.isAccepted("anyType"));
    u.isTrue(f.isAccepted("accept"));
    u.isTrue(f.isAccepted("accept.me"));
    u.isFalse(f.isAccepted("acceptme"));
    

    f.setDefault(true);
    u.isTrue(f.isAccepted("packet"));

    f.deny("packet");
    u.isFalse(f.isAccepted("packet"));
    u.isFalse(f.isAccepted("packet.network"));
    u.isTrue(f.isAccepted("node"));
    
    f.accept("notification.error");
    f.deny("notification");    
    u.isTrue(f.isAccepted("notification.error"));
    u.isTrue(f.isAccepted("notification.error.fatal"));
    u.isFalse(f.isAccepted("notification.hello"));    

    f.accept("notification");    
    u.isTrue(f.isAccepted("notification.hello"));    
    
    f.clear();
    u.isTrue(f.isAccepted("notification.hello"));    
    
    f.setDefault(false);
    u.isFalse(f.isAccepted("notification.hello"));        
}

} // end tests namespace
  
  
using namespace simulator::notification::tests;        

/**
 * @brief Run all the test for notification namespace
 */
void runTests() 
{
    cout << "---------------- START NOTIFICATION TESTS ----------------" << endl;
    testTypeFilter();
    cout << "---------------- END NOTIFICATION TESTS ----------------" << endl;
}
        
       
    }
}
