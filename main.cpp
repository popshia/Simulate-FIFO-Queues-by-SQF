// 10627130 資工二甲 林冠良 & 10627131 資工二甲 李峻瑋 // CodeBlocks 17.12
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <time.h>

using namespace std ;

static ifstream input ;
static ofstream output ;
static int QueueTime1 = 0 ;
static int QueueTime2 = 0 ;
static int FileN = 0 ;

typedef struct OrderStruct {
    int orderID = 0 ;
    int cookID = 0 ;
    int arrivalTime = 0 ;
    int duration = 0 ;
    int timeout = 0 ;
    int delay = 0 ;
    int abort = 0 ;
    int departure = 0 ;
    string whole = "\0" ;
} Data ;

class Functions {

    vector<OrderStruct> orders ;
    vector<OrderStruct> queue ;
    vector<OrderStruct> queue1 ;
    vector<OrderStruct> queue2 ;
    vector<OrderStruct> abort ;
    vector<OrderStruct> timeout ;
    string labels = "\0" ;
    int fail = 0 ;
    int totalDelay = 0 ;

public:
    void InputSort() {
        orders.clear() ;
        int dataCount = 0 ;
        string sentence = "\0" ;
        OrderStruct tempOrder ;
        getline( input, labels ) ;

        while ( getline( input, sentence ) ) {
            tempOrder.whole = sentence ; // save the whole data first
            vector<string> data ;
            string token ;
            istringstream cut( sentence ) ;
            while ( getline( cut, token, '\t' ) ) data.push_back( token ) ; // cut the data
            tempOrder.orderID = atoi( data[0].c_str() ) ; // set the first token to id
            tempOrder.arrivalTime = atoi( data[1].c_str() ) ; // set the second token to arrivalTime
            tempOrder.duration = atoi( data[2].c_str() ) ; // set the third token to duration
            tempOrder.timeout = atoi( data[3].c_str() ) ; // set the forth token to timeout
            orders.push_back( tempOrder ) ; // push tempOrder to the orders dataBase
            dataCount++ ;
        } // read and analyze the data
    } // inputSort()

    void ShellSort() {
        int dataCount = 0 ;
        string sentence = "\0" ;
        OrderStruct tempOrder ;
        orders.clear() ;
        getline( input, labels ) ; // get the labels

        clock_t read, readend ; // create a time variable
        clock_t sortTime, sortEnd ; // create a time variable
        clock_t write, writeEnd ; // create a time variable

        read = clock() ; // start the timer

        while ( getline( input, sentence ) ) {
            tempOrder.whole = sentence ; // save the whole data first
            vector<string> data ;
            string token ;
            istringstream cut( sentence ) ;
            while ( getline( cut, token, '\t' ) ) data.push_back( token ) ; // cut the data
            tempOrder.orderID = atoi( data[0].c_str() ) ; // set the first token to id
            tempOrder.arrivalTime = atoi( data[1].c_str() ) ; // set the second token to arrivalTime
            tempOrder.duration = atoi( data[2].c_str() ) ; // set the third token to duration
            tempOrder.timeout = atoi( data[3].c_str() ) ; // set the forth token to timeout
            orders.push_back( tempOrder ) ; // push tempOrder to the orders dataBase
            dataCount++ ;
        } // read and analyze the data

        readend = clock();
        read = readend - read ; // get the precise time

        cout << labels << endl ;
        for( int i = 0 ; i < orders.size() ; i ++ ) cout << orders[i].whole << endl ;

        sortTime = clock() ;

        OrderStruct temp ;
        for( int gap = dataCount/2 ; gap > 0 ; gap /= 2 ) {
            for( int i = gap ; i < dataCount ; i += 1 ) {
                temp = orders[i] ;
                int j ;
                for ( j = i ; j >= gap && orders[j-gap].arrivalTime > temp.arrivalTime ; j -= gap ) orders[j] = orders[j-gap] ;
                orders[j] = temp ;
            } // for()
        } // for()

        for ( int i = 0 ; i < orders.size() ; i ++ ) {
            if ( orders[i].arrivalTime == orders[i+1].arrivalTime && orders[i].orderID > orders[i+1].orderID ) swap( orders[i], orders[i+1] ) ;
        } // for()
        for ( int i = 0 ; i < orders.size() ; i ++ ) {
            if ( orders[i].arrivalTime == orders[i+1].arrivalTime && orders[i].orderID > orders[i+1].orderID ) swap( orders[i], orders[i+1] ) ;
        } // for()
        for ( int i = 0 ; i < orders.size() ; i ++ ) {
            if ( orders[i].arrivalTime == orders[i+1].arrivalTime && orders[i].orderID > orders[i+1].orderID ) swap( orders[i], orders[i+1] ) ;
        } // for()

        sortEnd = clock();
        sortTime = sortEnd - sortTime ; // get the precise time

        write = clock();

        if ( FileN == 401 ) output.open( "sort401.txt" ) ;
        else if ( FileN == 402 ) output.open( "sort402.txt" ) ;

        output << labels << endl ;
        for( int i = 0 ; i < orders.size() ; i ++ ) output << orders[i].whole << endl ;

        writeEnd = clock();
        write  = writeEnd - write;

        cout << endl;
        cout << "Reading data = " << read * 1000 / CLOCKS_PER_SEC << " ms" << endl << endl ; // print out the time
        cout << "Sorting data = " << sortTime * 1000 / CLOCKS_PER_SEC << " ms" << endl << endl ; // print out the time
        cout << "Writing data = " << write * 1000 / CLOCKS_PER_SEC << " ms" << endl << endl ; // print out the time

        cout << labels << endl ;
        for( int i = 0 ; i < orders.size() ; i ++ ) cout << orders[i].whole << endl ;
    } // shellSort()

    void Simulate() {
        orders.clear() ;
        abort.clear() ;
        timeout.clear() ;
        InputSort() ;
        int denominator = orders.size() ;
        fail = 0 ;
        totalDelay = 0 ;
        int queueTime = 0 ;
        double failPercent = 0 ;

        while ( orders.size() != 0 ) { // compare the tasks with the current queue time
            //cout << "Order Count is : " << orders.size() << endl ;
            //cout << "Current Order is : " << orders.front().orderID << " " << orders.front().arrivalTime << " " << orders.front().duration << " " << orders.front().timeout << endl ;
            //cout << "Current Queue Time is : " << queueTime << endl ;

            if ( orders.front().arrivalTime >= queueTime ) {
                if ( queue.empty() ) {
                    //cout << "First Order In : " << orders.front().orderID << endl << endl ;
                    queue.push_back( orders.front() ) ;
                    orders.erase( orders.begin() ) ;
                } // if the queue is empty, push the order in the queue

                if ( ! queue.empty() ) {
                    if ( queueTime < queue.front().arrivalTime ) queueTime = queue.front().arrivalTime ;
                    // push the time
                    if ( queueTime < queue.front().timeout ) {
                        queueTime += queue.front().duration ;
                        if ( queueTime > queue.front().timeout ) {
                            queue.front().delay = queueTime - queue.front().arrivalTime - queue.front().duration ;
                            totalDelay += queue.front().delay ;
                            queue.front().departure = queueTime ;
                            //cout << "push " << queue.front().orderID << " into timeout" << endl << endl ;
                            timeout.push_back( queue.front() ) ;
                            fail ++ ;
                        } // timeout orders
                    } // after the task is finished, found out that it run out of time
                    else {
                        queue.front().delay = queueTime - queue.front().arrivalTime ;
                        totalDelay += queue.front().delay ;
                        queue.front().abort = queueTime ;
                        //cout << "push " << queue.front().orderID << " into abort" << endl << endl ;
                        abort.push_back( queue.front() ) ;
                        fail ++ ;
                    } // when the task in the queue isn't feasible anymore, abort and count the fail
                    queue.erase( queue.begin() ) ;
                } // if the queue isn't empty, finish the tasks in the queue
            } // first order or orders that orders before current queue time

            else if ( orders.front().arrivalTime < queueTime ) {
                if ( queue.size() < 3 ) {
                    //cout << "push " << orders.front().orderID << " into queue" << endl << endl ;
                    queue.push_back( orders.front() ) ;
                } // push orders into queue
                else {
                    orders.front().delay = 0 ;
                    orders.front().abort = orders.front().arrivalTime ;
                    //cout << "push " << orders.front().orderID << " into abort" << endl << endl ;
                    abort.push_back( orders.front() ) ;
                    fail ++ ;
                } // arrival time greater or equal to current queue time
                orders.erase( orders.begin() ) ;
            } // check the queue to find out should the order be abort or push into the queue
        } // run orders and tasks

        while ( queue.size() != 0 ) {
            if ( queueTime < queue.front().arrivalTime ) queueTime = queue.front().arrivalTime ;
            // push the time
            if ( queueTime < queue.front().timeout ) {
                queueTime += queue.front().duration ;
                if ( queueTime > queue.front().timeout ) {
                    queue.front().delay = queueTime - queue.front().arrivalTime - queue.front().duration ;
                    totalDelay += queue.front().delay ;
                    queue.front().departure = queueTime ;
                    //cout << "push " << queue.front().orderID << " into timeout" << endl << endl ;
                    timeout.push_back( queue.front() ) ;
                    fail ++ ;
                } // time out orders
            } // after the task is finished, found out that it run out of time
            // feasible orders
            else {
                queue.front().delay = queueTime - queue.front().arrivalTime ;
                totalDelay += queue.front().delay ;
                queue.front().abort = queueTime ;
                //cout << "push " << queue.front().orderID << " into abort" << endl << endl ;
                abort.push_back( queue.front() ) ;
                fail ++ ;
            } // when the task in the queue isn't feasible anymore, abort and count the fail
            queue.erase( queue.begin() ) ;
        } // if the queue isn't empty, finish the tasks in the queue

        failPercent = ( (float)fail / (float)denominator ) * 100 ;
        if ( FileN == 401 ) output.open( "one401.txt" ) ;
        else if ( FileN == 402 ) output.open( "one402.txt" ) ;
        // print messages
        output << '\t' << "[Abort List]" << endl ;
        output << '\t' << "OID" << '\t' << "Delay" << '\t' << "Abort" << endl ;
        // output << "Abort List size is : " << abort.size() << endl ; // debug line
        for ( int i = 0 ; i < abort.size() ; i ++ ) output << "[" << i+1 << "]" << '\t' << abort[i].orderID << '\t' << abort[i].delay << '\t' << abort[i].abort << endl ;

        output << '\t' << "[Timeout List]" << endl ;
        output << '\t' << "OID" << '\t' << "Delay" << '\t' << "Departure" << endl ;
        // output << "Timeout List size is : " << timeout.size() << endl ; // debug line
        for ( int i = 0 ; i < timeout.size() ; i ++ ) output << "[" << i+1 << "]" << '\t' << timeout[i].orderID << '\t' << timeout[i].delay << '\t' << timeout[i].departure << endl ;

        output << "[Total Delay]" << endl ;
        output << totalDelay << " min." << endl ;
        output << "[Failure Percentage]" << endl ;
        // output << fail << " / " << denominator << " =" << endl ;
        output << setprecision(4) << failPercent << " %" << endl ;
    } // singleSimulate()

    void Queue1() {
        while ( orders.front().arrivalTime >= QueueTime1 ) {
            if ( queue1.empty() ) break ;

            if ( ! queue1.empty() ) {
                if ( QueueTime1 < queue1.front().arrivalTime ) QueueTime1 = queue1.front().arrivalTime ;
                // push the time
                if ( QueueTime1 < queue1.front().timeout ) {
                    QueueTime1 += queue1.front().duration ;
                    if ( QueueTime1 > queue1.front().timeout ) {
                        queue1.front().delay = QueueTime1 - queue1.front().arrivalTime - queue1.front().duration ;
                        totalDelay += queue1.front().delay ;
                        queue1.front().cookID = 1 ;
                        queue1.front().departure = QueueTime1 ;
                        //cout << "push " << queue1.front().orderID << " into timeout" << endl << endl ;
                        timeout.push_back( queue1.front() ) ;
                        fail ++ ;
                    } // timeout orders
                } // after the task is finished, found out that it run out of time
                else {
                    queue1.front().delay = QueueTime1 - queue1.front().arrivalTime ;
                    totalDelay += queue1.front().delay ;
                    queue1.front().cookID = 1 ;
                    queue1.front().abort = QueueTime1 ;
                    //cout << "push " << queue1.front().orderID << " into abort" << endl << endl ;
                    abort.push_back( queue1.front() ) ;
                    fail ++ ;
                } // when the task in the queue1 isn't feasible anymore, abort and count the fail
                queue1.erase( queue1.begin() ) ;
            } // if the queue1 isn't empty, finish the tasks in the queue1
        } // while()
    } // Queue1()

    void Queue2() {
        while ( orders.front().arrivalTime >= QueueTime2 ) {
            if ( queue2.empty() ) break ;

            if ( ! queue2.empty() ) {
                if ( QueueTime2 < queue2.front().arrivalTime ) QueueTime2 = queue2.front().arrivalTime ;
                // push the time
                if ( QueueTime2 < queue2.front().timeout ) {
                    QueueTime2 += queue2.front().duration ;
                    if ( QueueTime2 > queue2.front().timeout ) {
                        queue2.front().delay = QueueTime2 - queue2.front().arrivalTime - queue2.front().duration ;
                        totalDelay += queue2.front().delay ;
                        queue2.front().cookID = 2 ;
                        queue2.front().departure = QueueTime2 ;
                        //cout << "push " << queue2.front().orderID << " into timeout" << endl << endl ;
                        timeout.push_back( queue2.front() ) ;
                        fail ++ ;
                    } // timeout orders
                } // after the task is finished, found out that it run out of time
                else {
                    queue2.front().delay = QueueTime2 - queue2.front().arrivalTime ;
                    totalDelay += queue2.front().delay ;
                    queue2.front().cookID = 2 ;
                    queue2.front().abort = QueueTime2 ;
                    //cout << "push " << queue2.front().orderID << " into abort" << endl << endl ;
                    abort.push_back( queue2.front() ) ;
                    fail ++ ;
                } // when the task in the queue2 isn't feasible anymore, abort and count the fail
                queue2.erase( queue2.begin() ) ;
            } // if the queue2 isn't empty, finish the tasks in the queue2
        } // while()
    } // Queue2()

    void DoubleSimulate() {
        QueueTime1 = 0 ;
        QueueTime2 = 0 ;
        fail = 0 ;
        totalDelay = 0 ;
        orders.clear() ;
        abort.clear() ;
        timeout.clear() ;
        InputSort() ;
        int denominator = orders.size() ;
        double failPercent = 0 ;

        while ( ! orders.empty() ) {
            Queue1() ;
            Queue2() ;
            if ( queue1.size() == 3 && queue2.size() == 3 ) {
                orders.front().delay = 0 ;
                orders.front().cookID = 0 ;
                orders.front().abort = orders.front().arrivalTime ;
                //cout << "push " << orders.front().orderID << " into abort" << endl << endl ;
                abort.push_back( orders.front() ) ;
                fail ++ ;
            } // abort

            else {
                if ( ( orders.front().arrivalTime >= QueueTime1 && orders.front().arrivalTime >= QueueTime2 ) || ( orders.front().arrivalTime < QueueTime1 && orders.front().arrivalTime < QueueTime2 ) ) {
                    if ( queue1.size() == queue2.size() ) {
                        orders.front().cookID = 1 ;
                        //cout << "push " << orders.front().orderID << " into queue1" << endl << endl ;
                        queue1.push_back( orders.front() ) ;
                    } // same size

                    else if( queue1.size() < queue2.size() ) {
                        orders.front().cookID = 1 ;
                        //cout << "push " << orders.front().orderID << " into queue1" << endl << endl ;
                        queue1.push_back( orders.front() ) ;
                    } // cook1

                    else if( queue1.size() > queue2.size() ) {
                        orders.front().cookID = 2 ;
                        //cout << "push " << orders.front().orderID << " into queue2" << endl << endl ;
                        queue2.push_back( orders.front() ) ;
                    } // cook1
                } // if()

                else if ( orders.front().arrivalTime >= QueueTime1 && orders.front().arrivalTime < QueueTime2 ) {
                    queue1.push_back( orders.front() ) ;
                    //cout << "push " << orders.front().orderID << " into queue1" << endl << endl ;
                } // else if

                else if ( orders.front().arrivalTime >= QueueTime2 && orders.front().arrivalTime < QueueTime1 ) {
                    queue2.push_back( orders.front() ) ;
                    //cout << "push " << orders.front().orderID << " into queue2" << endl << endl ;
                } // else if
            } // else()

            Queue1() ;
            Queue2() ;
            orders.erase( orders.begin() ) ;
        } // run the orders

        /*for ( int i = 0 ; i < queue1.size() ; i++ ) {
            cout << queue1[i].whole << endl ;
        } // debug
        cout << endl ;
        for ( int i = 0 ; i < queue2.size() ; i++ ) {
            cout << queue2[i].whole << endl ;
        } // debug */

        while ( ! ( queue1.empty() && queue2.empty() ) ) {
            if ( queue1.empty() && ! queue2.empty() ) {
                if ( QueueTime2 < queue2.front().arrivalTime ) QueueTime2 = queue2.front().arrivalTime ;
                // push the time
                if ( QueueTime2 < queue2.front().timeout ) {
                    QueueTime2 += queue2.front().duration ;
                    if ( QueueTime2 > queue2.front().timeout ) {
                        queue2.front().delay = QueueTime2 - queue2.front().arrivalTime - queue2.front().duration ;
                        totalDelay += queue2.front().delay ;
                        queue2.front().cookID = 2 ;
                        queue2.front().departure = QueueTime2 ;
                        //cout << "push " << queue2.front().orderID << " into timeout" << endl << endl ;
                        timeout.push_back( queue2.front() ) ;
                        fail ++ ;
                    } // timeout orders
                } // after the task is finished, found out that it run out of time
                else {
                    queue2.front().delay = QueueTime2 - queue2.front().arrivalTime ;
                    totalDelay += queue2.front().delay ;
                    queue2.front().cookID = 2 ;
                    queue2.front().abort = QueueTime2 ;
                    //cout << "push " << queue2.front().orderID << " into abort" << endl << endl ;
                    abort.push_back( queue2.front() ) ;
                    fail ++ ;
                } // when the task in the queue2 isn't feasible anymore, abort and count the fail
                queue2.erase( queue2.begin() ) ;
            }

            else if ( ! queue1.empty() && queue2.empty() ) {
                if ( QueueTime1 < queue1.front().arrivalTime ) QueueTime1 = queue1.front().arrivalTime ;
                // push the time
                if ( QueueTime1 < queue1.front().timeout ) {
                    QueueTime1 += queue1.front().duration ;
                    if ( QueueTime1 > queue1.front().timeout ) {
                        queue1.front().delay = QueueTime1 - queue1.front().arrivalTime - queue1.front().duration ;
                        totalDelay += queue1.front().delay ;
                        queue1.front().cookID = 1 ;
                        queue1.front().departure = QueueTime1 ;
                        //cout << "push " << queue1.front().orderID << " into timeout" << endl << endl ;
                        timeout.push_back( queue1.front() ) ;
                        fail ++ ;
                    } // timeout orders
                } // after the task is finished, found out that it run out of time
                else {
                    queue1.front().delay = QueueTime1 - queue1.front().arrivalTime ;
                    totalDelay += queue1.front().delay ;
                    queue1.front().cookID = 1 ;
                    queue1.front().abort = QueueTime1 ;
                    //cout << "push " << queue1.front().orderID << " into abort" << endl << endl ;
                    abort.push_back( queue1.front() ) ;
                    fail ++ ;
                } // when the task in the queue1 isn't feasible anymore, abort and count the fail
                queue1.erase( queue1.begin() ) ;
            }

            else if ( QueueTime1 <= QueueTime2 ) {
                if ( QueueTime1 < queue1.front().arrivalTime ) QueueTime1 = queue1.front().arrivalTime ;
                // push the time
                if ( QueueTime1 < queue1.front().timeout ) {
                    QueueTime1 += queue1.front().duration ;
                    if ( QueueTime1 > queue1.front().timeout ) {
                        queue1.front().delay = QueueTime1 - queue1.front().arrivalTime - queue1.front().duration ;
                        totalDelay += queue1.front().delay ;
                        queue1.front().cookID = 1 ;
                        queue1.front().departure = QueueTime1 ;
                        //cout << "push " << queue1.front().orderID << " into timeout" << endl << endl ;
                        timeout.push_back( queue1.front() ) ;
                        fail ++ ;
                    } // timeout orders
                } // after the task is finished, found out that it run out of time
                else {
                    queue1.front().delay = QueueTime1 - queue1.front().arrivalTime ;
                    totalDelay += queue1.front().delay ;
                    queue1.front().cookID = 1 ;
                    queue1.front().abort = QueueTime1 ;
                    //cout << "push " << queue1.front().orderID << " into abort" << endl << endl ;
                    abort.push_back( queue1.front() ) ;
                    fail ++ ;
                } // when the task in the queue1 isn't feasible anymore, abort and count the fail
                queue1.erase( queue1.begin() ) ;
            } // queue1 first

            else if ( QueueTime2 < QueueTime1 ) {
                if ( QueueTime2 < queue2.front().arrivalTime ) QueueTime2 = queue2.front().arrivalTime ;
                // push the time
                if ( QueueTime2 < queue2.front().timeout ) {
                    QueueTime2 += queue2.front().duration ;
                    if ( QueueTime2 > queue2.front().timeout ) {
                        queue2.front().delay = QueueTime2 - queue2.front().arrivalTime - queue2.front().duration ;
                        totalDelay += queue2.front().delay ;
                        queue2.front().cookID = 2 ;
                        queue2.front().departure = QueueTime2 ;
                        //cout << "push " << queue2.front().orderID << " into timeout" << endl << endl ;
                        timeout.push_back( queue2.front() ) ;
                        fail ++ ;
                    } // timeout orders
                } // after the task is finished, found out that it run out of time
                else {
                    queue2.front().delay = QueueTime2 - queue2.front().arrivalTime ;
                    totalDelay += queue2.front().delay ;
                    queue2.front().cookID = 2 ;
                    queue2.front().abort = QueueTime2 ;
                    //cout << "push " << queue2.front().orderID << " into abort" << endl << endl ;
                    abort.push_back( queue2.front() ) ;
                    fail ++ ;
                } // when the task in the queue2 isn't feasible anymore, abort and count the fail
                queue2.erase( queue2.begin() ) ;
            } // queue2 first
        } // deal the rest*/

        failPercent = ( (float)fail / (float)denominator ) * 100 ;
        if ( FileN == 401 ) output.open( "two401.txt" ) ;
        else if ( FileN == 402 ) output.open( "two402.txt" ) ;
        // print messages
        output << '\t' << "[Abort List]" << endl ;
        output << '\t' << "OID" << '\t' << "CID" << '\t' << "Delay" << '\t' << "Abort" << endl ;
        // output << "Abort List size is : " << abort.size() << endl ; // debug line
        for ( int i = 0 ; i < abort.size() ; i ++ ) output << "[" << i+1 << "]" << '\t' << abort[i].orderID << '\t' << abort[i].cookID << '\t' << abort[i].delay << '\t' << abort[i].abort << endl ;

        output << '\t' << "[Timeout List]" << endl ;
        output << '\t' << "OID" << '\t' << "CID" << '\t' << "Delay" << '\t' << "Departure" << endl ;
        // output << "Timeout List size is : " << timeout.size() << endl ; // debug line
        for ( int i = 0 ; i < timeout.size() ; i ++ ) output << "[" << i+1 << "]" << '\t' << timeout[i].orderID << '\t' << timeout[i].cookID << '\t' << timeout[i].delay << '\t' << timeout[i].departure << endl ;

        output << "[Total Delay]" << endl ;
        output << totalDelay << " min." << endl ;
        output << "[Failure Percentage]" << endl ;
        // output << fail << " / " << denominator << " =" << endl ;
        output << setprecision(4) << failPercent << " %" << endl ;
    } // doubleSimulate()
} ;

int main() {

    int command = 0 ;
    bool continueOrNot = false ;
    Functions shellSort ;
    Functions simulate ;
    Functions doubleSimulate ;

    do {
        cout << "*****************************************" << endl ; // welcome message
        cout << "***    Simulate FIFO Queues by SQF    ***" << endl ;
        cout << "*** 0 : Quit                          ***" << endl ;
        cout << "*** 1 : Sort a file                   ***" << endl ;
        cout << "*** 2 : Simulate one FIFO queue       ***" << endl ;
        cout << "*** 3 : Simulate two queues by SQF    ***" << endl ;
        cout << "*****************************************" << endl ;
        cout << endl << "Please enter your choice :" << endl ;

        cin >> command ; // read in user command

        if ( command == 0 ) { // bye :(((
            cout << "Bye :(((" << endl ;
            return 0 ;
        } // quit

        else if ( command > 3 || command < 0 ) {
            cout << "Error command! please enter an acceptable command :" << endl << endl ;
            continueOrNot = true ;
        } // wrong command

        else if ( command == 1 ) {

            bool function1Confirm = false ;

            do {
                cout << "Please enter the file you want to sort or [0] to quit:" << endl ;
                cin >> FileN ;

                if ( FileN == 0 ) {
                    function1Confirm = true ;
                    continueOrNot = true ;
                } // quit

                else if ( FileN != 401 && FileN != 402  )
                    cout << "*****  input" << FileN << ".txt does not exist!  *****" << endl ;

                else if ( FileN == 401 ) {
                    input.open( "input401.txt" ) ;
                    if ( ! input.is_open() ) cout << "*****  input401.txt does not exist!  *****" << endl ;

                    else {
                        function1Confirm = true ;
                        shellSort.ShellSort() ;
                        continueOrNot = true ;
                    } // find input 401
                } // test if you have already create a sort file

                else if ( FileN == 402 ) {
                    input.open( "input402.txt" ) ;
                    if ( ! input.is_open() ) cout << "*****  input402.txt does not exist!  *****" << endl ;

                    else {
                        function1Confirm = true ;
                        shellSort.ShellSort() ;
                        continueOrNot = true ;
                    } // find input 402
                } // test if you have already create a sort file

            } while ( ! function1Confirm ) ;

            FileN = 0 ;
            input.close() ;
            output.close() ;
        } // mission 2

        else if ( command == 2 ) {

            bool function2Confirm = false ;

            do {
                cout << "Please enter the file you want to simulate or [0] to quit:" << endl ;
                cin >> FileN ;

                if ( FileN == 0 ) {
                    function2Confirm = true ;
                    continueOrNot = true ;
                } // quit

                else if ( FileN != 401 && FileN != 402  )
                    cout << "*****  sort" << FileN << ".txt does not exist!  *****" << endl ;

                else if ( FileN == 401 ) {
                    input.open( "sort401.txt" ) ;
                    if ( ! input.is_open() ) cout << "*****  sort401.txt does not exist!  *****" << endl ;

                    else {
                        function2Confirm = true ;
                        simulate.Simulate() ;
                        continueOrNot = true ;
                    } // find sort 401
                } // test if you have already create a sort file

                else if ( FileN == 402 ) {
                    input.open( "sort402.txt" ) ;
                    if ( ! input.is_open() ) cout << "*****  sort402.txt does not exist!  *****" << endl ;

                    else {
                        function2Confirm = true ;
                        simulate.Simulate() ;
                        continueOrNot = true ;
                    } // find sort 402
                } // test if you have already create a sort file

            } while ( ! function2Confirm ) ;

            FileN = 0 ;
            input.close() ;
            output.close() ;

        } // mission 2

        else if ( command == 3 ) {

            bool function3Confirm = false ;

            do {
                cout << "Please enter the file you want to simulate or [0] to quit:" << endl ;
                cin >> FileN ;

                if ( FileN == 0 ) {
                    function3Confirm = true ;
                    continueOrNot = true ;
                } // quit

                else if ( FileN != 401 && FileN != 402  )
                    cout << "*****  sort" << FileN << ".txt does not exist!  *****" << endl ;

                else if ( FileN == 401 ) {
                    input.open( "sort401.txt" ) ;
                    if ( ! input.is_open() ) cout << "*****  sort401.txt does not exist!  *****" << endl ;

                    else {
                        function3Confirm = true ;
                        doubleSimulate.DoubleSimulate() ;
                        continueOrNot = true ;
                    } // find sort 401
                } // test if you have already create a sort file

                else if ( FileN == 402 ) {
                    input.open( "sort402.txt" ) ;
                    if ( ! input.is_open() ) cout << "*****  sort402.txt does not exist!  *****" << endl ;

                    else {
                        function3Confirm = true ;
                        doubleSimulate.DoubleSimulate() ;
                        continueOrNot = true ;
                    } // find sort 402
                } // test if you have already create a sort file

            } while ( ! function3Confirm ) ;

            FileN = 0 ;
            input.close() ;
            output.close() ;

        } // mission 3

    } while( continueOrNot ) ;
} // main()
