#include <iostream>
#include <fstream>

#include "Simulator.h"
#include "Units.h"
#include "Command.h"
#include "common.h"
#include "Notification.h"
#include "TypeFilter.h"
#include "Exceptions.h"
#include "CommandQueryResult.h"
#include "HypercubeNetwork.h"

namespace simulator {

using namespace std;
using namespace simulator::command;
using namespace simulator::hypercube;
using namespace simulator::notification;

Simulator* Simulator::instance = NULL;

/**
 * @brief Create a new Simulator.  It's private to enforce the singleton.
 */
Simulator::Simulator()
{
    TypeFilter *tf = new TypeFilter();
    tf->accept("simulator.exec.query");

    notifFilter = tf;
    percent = 1;
    showProgress = false;
    network = new HypercubeNetwork();
}

/**
 * @brief Destroy the instance.  A new one will be created the next time it is accessed.
 */
void Simulator::destroy()
{
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

/**
 * @brief Reset the simulator time.
 */
void Simulator::reset()
{
    if (instance != NULL) {
        instance->time = 0;
        instance->percent = 1;
    }
}

/**
 * @brief Get the unique instance of the simulator.
 *
 * @return the unique instance of the simulator.
 */
Simulator *Simulator::getInstance()
{
    if (instance == NULL) {
        instance = new Simulator();
    }
    return instance;
}

/**
 * @brief Get the time in the simulation.
 *
 * @return the time in the simulation.
 */
Time Simulator::getTime() const
{
    return time;
}

/**
 * @brief Simulate one step (run just an event).
 *
 * @param maxTime if specified and the current time is later than this time, it doesn't run any event.
 * @return true if the queue was not empty and a step was simulated.
 */
bool Simulator::simulateStep(Time maxTime)
{
    if (eventQueue.empty()) {
        return false;
    }

    if (showProgress) {
        double p = (100.0 * time.getValue() / maxTime.getValue() );
        if (p > percent) {
              cout << ((int) p) << "% ";
              percent = (int) p + 1;
        }
    }

    // get the next event from queue.
    TEvent *e =  eventQueue.top();
    eventQueue.pop();

    time = e->getTime();

    if ((maxTime.getValue() > 0) && (time > maxTime)) return false;

    // run the event and reschedule if it is periodic
    e->runEvent();

    // if the event is not periodic, delete from memory
    if (e->getPeriod().getValue()== 0)  delete e;

    return true;
}

/**
 * @brief Add an event to the simulator queue.
 *
 * @param event event to add.
 * @param timeRelative whether the time should be added to the current time or is absoulte.
 */
void Simulator::addEvent(TEvent *event, bool timeRelative)
{
    if (timeRelative) {
        event->addTime(time);
    }
    eventQueue.push(event);
}

/**
 * @brief Notify something without any values on it.
 *
 * @param notificationType type of notification.
 * @param node node where the notification happens.
 */
void Simulator::notify(const string &notificationType, TNode *node)
{
    if (notifFilter->isAccepted(notificationType)) {
        writeNotification(notificationType, NULL, "node", node->getId());
    }
}


/**
 * @brief Notify something by querying an object.
 *
 * @param notificationType type of notification.
 * @param object object to query
 * @param queryOptions options to pass to the object when querying it
 * @param node node where the notification happens.
 */
void Simulator::notify(const string &notificationType, const TQueryable *object,vector<string> *queryOptions, TNode *node)
{
    if (notifFilter->isAccepted(notificationType)) {
        writeNotification(notificationType, object->query(queryOptions), "node", node->getId());
    }
}

/**
 * @brief Notify something by using a query already obtained.
 *
 * @param notificationType type of notification.
 * @param queryResult result to write in the notification
 * @param node node where the notification happens.
 */
void Simulator::notify(const string &notificationType, QueryResult *queryResult, TNode *node)
{
    if (notifFilter->isAccepted(notificationType)) {
        if (node != NULL) writeNotification(notificationType, queryResult, "node", node->getId());
        else writeNotification(notificationType, queryResult);
    } else {
        delete queryResult;
    }
}
/**
 * @brief Notify something by using a query already obtained plus an extra value.
 *
 * @param notificationType type of notification.
 * @param queryResult result to write in the notification
 * @param extraDataName name of the extra data
 * @param extraDataValue value of the extra data
 */
void Simulator::notify(const string &notificationType, QueryResult *queryResult, const string &extraDataName, const string &extraDataValue)
{
    if (notifFilter->isAccepted(notificationType)) {
        writeNotification(notificationType, queryResult, extraDataName,extraDataValue);
    } else {
        delete queryResult;
    }
}

/**
 * @brief Write a notification about something by using a query already obtained plus an extra value.
 * Notice that it doesn't check if the type of the notification is accepted or not, as notify method do.
 *
 * @param notificationType type of notification.
 * @param queryResult result to write in the notification
 * @param extraDataName name of the extra data
 * @param extraDataValue value of the extra data
 */
void Simulator::writeNotification(const string &notificationType, QueryResult *queryResult, const string &extraDataName, const string &extraDataValue)
{
    QueryResult *qr = new QueryResult("Notification", notificationType);
    qr->insert("time", getTime().toString(Time::SEC));

    if (extraDataName.length() > 0) {
        qr->insert(extraDataName, extraDataValue);
    }

    if (queryResult != NULL) qr->insert("", queryResult);
    notificator.write(qr);
    delete qr;
}

/**
 * @brief Execute a command in the network.
 *
 * @param cmd command to execute
 */
void Simulator::exec(const string &cmd)
{
    exec(network, cmd);
}

/**
 * @brief Execute a command in the specified destination
 *
 * @param dest where the command is executed
 * @param cmd command to execute
 */
void Simulator::exec(TCommandRunner *dest, const string &cmd)
{
    TCommandResult *cr;
    try {
         cr = Command(cmd).run(dest);
    } catch (exception &e) {
        QueryResult qr("Error", "ErrorMessage", e.what());
        notificator.write(&qr);
        throw;
    }


    CommandQueryResult *cqr = dynamic_cast<CommandQueryResult *>(cr);

    if (cqr != NULL) {
        notify("simulator.exec.query", cqr->getQueryResult(), "command", cmd);
        delete cr; // valgrind
    } else {
        notify("simulator.exec", NULL, "command", cmd);
    }
}

/**
 * @brief Set the notification filter used.
 *
 * @param notifFilter the notification filter used.
 */
void Simulator::setNotifFilter(TNotifFilter *notifFilter)
{
    this->notifFilter = notifFilter;
}

/**
 * @brief Get the notification filter used.
 *
 * @return the notification filter used.
 */
TNotifFilter *Simulator::getNotifFilter() const
{
    return notifFilter;
}

/**
 * @brief Set the network in use.
 *
 * @param network the network in use.
 */
void Simulator::setNetwork(TNetwork *network)
{
    this->network = network;
}

/**
 * @brief Get the Notificator.
 *
 * @return the Notificator.
 */
Notificator &Simulator::getNotificator() 
{ 
    return notificator; 
} 

/**
 * @brief Get the network in use.
 *
 * @return the network in use.
 */
TNetwork *Simulator::getNetwork() const
{
    return network;
}

/**
 * @brief Run a simulation until the end time or no more events are scheduled.
 */
void Simulator::simulate()
{
    while(simulateStep(endTime));

    // clean events not executed    
    while (!eventQueue.empty()) {
        TEvent *e =  eventQueue.top();
        eventQueue.pop();
        delete e;
    }
    
}

/**
 * @brief Load a simulator file and schedules all its commands.
 *
 * @param fileName name of the file to load.
 */
void Simulator::loadFile(const string &fileName)
{
    ifstream file;
    file.open(fileName.c_str());

    if (!file) throw invalid_argument("Unable to open file");

    char aux[1024];
    Time t = -1;
    int p;
    int lineNumber = 1;
    while (file.getline(aux, 1024)) {
        string line(aux);

        // Remove commentaries
        p = line.find('#');
        if (p >= 0) line = line.substr(0, p);

        // Remove empty spaces at the beginning and end
        line = trim(line);

        // Extract the time
        if (line[0] == '[') {
            p = line.find(']');
            if (p < 0) throw invalid_argument("Missing ']' in line " + toStr(lineNumber));
            t = Time(trim(line.substr(1, p-1)));

            if (t > endTime) endTime = t;

            line = trim(line.substr(p + 1));

        }

        // skip if there is no command to process
        if (line.size() == 0) continue;

        if (t.getValue() < 0) {
            exec(line);
        } else {
            addEvent(new CommandRunnerEvent(t, getNetwork(), line));
        }


        lineNumber++;
    }
    file.close();
}

/**
 * @brief Set the end time for the simulation.
 *
 * @param t end time for the simulation.
 */
void Simulator::setEndTime(Time t)
{
    endTime = t;
}


/**
 * @brief Run a command.
 *
 * @param function function to run.
 */
TCommandResult *Simulator::runCommand(const Function &f)
{
    if (f.getName() == "notifFilter")
    {
        return notifFilter;
    }

    if (f.getName() == "formatter") {
        return getNotificator().getFormatter();
    }

    throw command_error("Simulator - Bad function: " + f.toString());
}

/**
 * @brief Get this object name.
 *
 * @return "simulator"
 */
string Simulator::getName() const
{
    return "simulator";
}

/**
 * @brief Whether to show simulation progress.
 *
 * @param show true to show the progress.
 */
void Simulator::setShowProgress(bool show)
{
     showProgress = show;
}



}
