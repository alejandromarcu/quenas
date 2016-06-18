#include <cmath>

#include "UnitTest.h"
#include "MockCommand.h"
#include "Notification.h"
#include "Connection.h"
#include "MockLayer.h"
#include "UniversalAddress.h"
#include "HypercubeNode.h"

namespace simulator {
    namespace command {

      
namespace tests {

using namespace simulator::layer;
using namespace simulator::address;
using namespace simulator::layer::tests;

using namespace simulator::notification;

/** 
 * @brief Test some simple commands
 */
void testCommand1() 
{
    UnitTest u("testCommand1");

    MockCommandRunner root("root");
    MockCommandRunner child1("child1");
    MockCommandRunner child2("child2");    
    MockCommandRunner grandchild1("grandchild1");

    root.addChild(&child1);
    root.addChild(&child2);     
    child1.addChild(&grandchild1);
    
    TCommandResult *cr;    
    Command("setNoParam").run(&root);
    u.areEqual("NoParam", root.getValue(), "cmd 'setNoParam'");

    Command("setNoParam()").run(&root);
    u.areEqual("NoParam", root.getValue(), "cmd 'setNoParam()'");
    
    Command("setValue(this is a test)").run(&root);    
    u.areEqual("this is a test", root.getValue(), "cmd 'setValue(this is a test)'");

    Command("  setValue  (this is a test)   ").run(&root);    
    u.areEqual("this is a test", root.getValue(), "cmd '   setValue  (this is a test)   '");

    Command("setValue('  quoted.string(((.'  )").run(&root);    
    u.areEqual("  quoted.string(((.", root.getValue(), "setValue('  quoted.string(((.'  )");

    Command("setValue(this.is.a.test)").run(&root);    
    u.areEqual("this.is.a.test", root.getValue(), "cmd 'setValue(this.is.atest)'");

    Command("setValue(ab().cd())").run(&root);    
    u.areEqual("ab().cd()", root.getValue(), "setValue(ab().cd())");

    Command("setInt(value, 26)").run(&root);    
    u.areEqual("value", root.getValue(), "cmd 'setInt(value, 26 )' [value]");
    u.areEqual(26, root.getIntValue(), "cmd 'setInt(value, 26 )' [int]");


    Command("setBool(t,  true  )").run(&root);    
    u.areEqual("t", root.getValue(), "cmd 'setBool(t, true )' [value]");
    u.isTrue(root.getBoolValue(), "cmd 'setBool(t, true )' [bool]");
   
    cr = Command("get(child1)").run(&root);
    u.isTrue(cr == &child1, "didn't get child1");

    cr = Command("get(child2)").run(&root);
    u.isTrue(cr == &child2, "didn't get child2");

    cr = Command("get(child1).get(grandchild1)").run(&root);
    u.isTrue(cr == &grandchild1, "didn't get grandchild1");

    cr = Command("get(child1).get(grandchild1).setValue(hi)").run(&root);
    u.areEqual("hi", grandchild1.getValue(), "cmd 'get(child1).get(grandchild1).setValue(hi)'");

}

/**
 * @brief test using multiCommand (a command that returns multiple values)
 */
void testMultiCommand() 
{
    UnitTest u("testMultiCommand");
    
    TCommandResult *cr;
    
    MockCommandRunner root("root");
    MockCommandRunner child1("child1");
    MockCommandRunner child2("child2");    
    MockCommandRunner grandchild1("grandchild1");

    root.addChild(&child1);
    root.addChild(&child2);     
    child1.addChild(&grandchild1);

    MockCommandRunner a1("a");
    MockCommandRunner a2("a");    
    MockCommandRunner b1("b");
    MockCommandRunner b2("b");    
    child1.addChild(&a1);
    child1.addChild(&b1);
    child2.addChild(&a2);
    child2.addChild(&b2);

    cr = Command("getMany(child1,child2).setValue(many)").run(&root);
    u.areEqual("many", child1.getValue());
    u.areEqual("many", child2.getValue());    

    
    cr = Command("getMany(child1,child2).getMany(a,b).setValue(MM)").run(&root);
    
    u.areEqual("MM", a1.getValue());
    u.areEqual("MM", a2.getValue());
    u.areEqual("MM", b1.getValue());
    u.areEqual("MM", b2.getValue());            
}


} // end tests namespace
  
  
using namespace simulator::command::tests;        

/**
 * @brief Run all the test for layer namespace
 */
void runTests() 
{
    cout << "---------------- START COMMAND TESTS ----------------" << endl;
    testCommand1(); 
    testMultiCommand();    
    cout << "---------------- END COMMAND TESTS ----------------" << endl;
}
        
       
    }
}
