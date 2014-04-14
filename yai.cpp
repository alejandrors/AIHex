/** =========================================================
  *
  * University of Alberta
  * Department of Computer Science
  * CMPUT 670 - Games, graphs and algorithms
  *
  * Author:	Alejandro Ramirez Sanabria
        (ramirezs@ualberta.ca)
  * Date:	3/FEB/2014
  *
  * Abstract:	Assignment 2 - Implementation for different
  *		AI players defined in the header file.
  * =========================================================
*/

#include "yai.h"
#include "ygui.h"

using namespace ARS;

//This function simulates a tournament between two different AIs.


double YAI::testAgainst(YAI * __opponent, unsigned __iterations) {
    if(__opponent->getBoardSize() != this->getBoardSize()) {
        YException(CommandInvalid);
    }
    unsigned _wins= 0;
    unsigned _iter = __iterations;
    GameState _gs(this->boardSize);
    list<PlayerMove>  _history;
    //YGUI gui;
    bool _check = true;
    while(_iter-- > 0) {
        bool _thisFirstPlay = rand()%2;
        //bool _thisFirstPlay = true;
        //while(!_gs.isFull()) {
        while(_gs.fullReduce()==EMPTY_CELL){
            if (_thisFirstPlay) {

                Coords _move = this->makeMove(&_gs, &_history);
                _check &= _gs.markCell(_move.x, _move.y, this->getCurrentPlayer());
                _history.push_front(PlayerMove(_move, this->getCurrentPlayer()));
                _move = __opponent->makeMove(&_gs, &_history);
                if (_move.isValid()) {
                    _check &=_gs.markCell(_move.x, _move.y, __opponent->getCurrentPlayer());
                    _history.push_front(PlayerMove(_move, __opponent->getCurrentPlayer()));
                }
            } else {
                Coords _move = __opponent->makeMove(&_gs, &_history);
                _check &=_gs.markCell(_move.x, _move.y, __opponent->getCurrentPlayer());
                _history.push_front(PlayerMove(_move, __opponent->getCurrentPlayer()));
                _move = this->makeMove(&_gs, &_history);
                if(_move.isValid()) {
                    _check &=_gs.markCell(_move.x, _move.y, this->getCurrentPlayer());
                    _history.push_front(PlayerMove(_move, this->getCurrentPlayer()));
                }
            }
            if(!_check) {
                throw YException(BadCoordinates);
            }
            //gui.drawBoard(&_gs);

        }
        if (_gs.fullReduce()==this->getCurrentPlayer()) {
            _wins++;
        } else  {
            _wins = _wins;
        }
        _gs.setBoard(this->boardSize);
        _history.clear();
    }
    return ((double)_wins/(double)__iterations);
}


/***********  RANDOM AI AGENT **************/
//Random move generator -- selects from free spaces.

bool YAI_Random::setParams(AIParams __params, double __value) {
    switch(__params) {
        case BoardSize:
        this->boardSize = (unsigned)floor(__value);
            return true;
            break;
        default:
            return false;
    }
}

Coords YAI_Random::makeMove(GameState * __gs, list<PlayerMove>* _p) {
    //srand (time(NULL));
    list<Coords>  _empty = __gs->getEmptyCells();
    int _iElem = 0;
    if (_empty.size()) {
        _iElem = rand()%_empty.size();
        list<Coords>::iterator _elem = _empty.begin();
        while (_iElem--!=0) { _elem++;}
        return Coords(*_elem);
    }

    return Coords(0,0);
}


//Reinforcement learning SARSA Lambda with Function Approximation.

Coords YAI_SARSA_RL::makeMove(GameState * __gs, list<PlayerMove>*) {
    if (!initialized) {
        return Coords(0,0);
    }

    loadF(__gs);
    /*cout << "F" << endl;
    cout << (*features);
    cout << endl;*/
    loadQ();
    cout << "Q" << endl;
    cout << (*q);
    cout << endl;
    int _ind = takeAction(__gs, false);
    if (_ind < 0) {
        return Coords(0,0);
    }
    return __gs->getCoordPair(_ind);
}

void YAI::initLogs(const char * _logPath, const char * _csvPath) {
    if(logging) {
        logHandle = init_log(STR_FILENAME_RLAILOG, false);
        csvHandle = init_csv(STR_FILENAME_RLAICSV);
    }
}

//Initializer function that sets all the default values

bool YAI_SARSA_RL::init(unsigned __boardSize) {
    srand (time(NULL));
    reset(); 		//WARNING: init should not be called arbitrarily.
    initLogs(STR_FILENAME_RLAILOG, STR_FILENAME_RLAICSV);
    this->initialized = true;
    this->numIterations = 0;
    this->boardSize = __boardSize;
    this->boardCellNum = (__boardSize*(__boardSize+1))/2;
    this->featureSize = 3*this->boardCellNum*this->boardCellNum; //ceil(log(this->boardCellNum)/log(2)); //Incorporate the action too.
    this->currentPlayer = BLACK_CELL;
    e = new Vector(this->featureSize);
    features = new Matrix(this->boardCellNum,featureSize,0);
    theta = new Vector(this->featureSize);
    int i= 0;
    while(i<this->featureSize) {
        (*theta)(i) = 0.1; //(double)(rand()%10);
        //cout << (*theta)(i++) << endl;
        i++;
    }
    q = new Vector(this->boardCellNum);
    return true;
}

YAI_SARSA_RL::~YAI_SARSA_RL() {
    reset();
}

int YAI_SARSA_RL::takeAction(GameState * __gs, bool __isRandom) {
    list<Coords> _freeCells = __gs->getEmptyCells();
    unsigned _ties = 1;
    if (!_freeCells.empty()) {
        list<Coords>::iterator _pos = _freeCells.begin();
        if (__isRandom && epsilonGreedy(epsilon)) {
            //cout << "Taking random action";
            //cout.flush();
            int _times = (rand() % _freeCells.size());
            while(_times-- > 0) _pos++;
        } else {
            unsigned _ind = __gs->getAbsPos(*_freeCells.begin()); //A zero based object
            double _temp, _max = (*q)(_ind);
            list<Coords>::iterator I = _freeCells.begin();
            I++;
            for(list<Coords>::iterator E = _freeCells.end(); I!=E; ++I) {
                _ind = __gs->getAbsPos(*I);
                _temp = (*q)(_ind);

                if (_temp >= _max) {
                    if (_temp > _max) {
                        _ties = 1;
                        _max = _temp;
                        _pos = I;
                    } else {
                        _ties++;
                        if (0 == rand() % _ties) {
                            _max = _temp;
                            _pos = I;
                        }
                    }
                }
            }
        }
        return __gs->getAbsPos(*_pos);
    } else {
        return -1; 	//No actions left to take.
    }
}

//This code is based on Rich Sutton's book for SARSA Lambda

double YAI_SARSA_RL::runEpisode(GameState * __gs, YAI * __opponent) {
    using namespace boost::numeric::ublas;
    unsigned i, _step = 0;
    int _action = -1;
    float _temp;
    double _rewardTotal = 0;
    int _delta,_reward = 0;
    list<PlayerMove> _moves;
    bool _historyOn = false;



    //Decide which player goes first. Prepare the initial state with a  move the by the other user if necessary.
    bool _thisPlayer = rand()%2;

    do
    {
        GameState _gs(*__gs);

        _step++;

        list<PlayerMove>::reverse_iterator I = _moves.rbegin();
        list<PlayerMove>::reverse_iterator E = _moves.rend();
        //Clear the eligibility traces TODO: Check of this can be replaced with the boost library zero_vector()
        i=0;
        while(i < this->featureSize) (*e)(i++) = 0;

        //bool _thisPlayer = true;
        if (!_thisPlayer) {
            generateAfterState(&_gs, __opponent, -1, _thisPlayer, I, &_moves, _historyOn);
        }

        loadF(&_gs);
        loadQ();

        //Take optimal action and change it with probability 1- epsilon
        _action = takeAction(&_gs, true);

        //Iterate until the end of an episode.
        do {



            i=0;
            //Replacing traces -- decay
            while(i < this->featureSize) {
                (*e)(i) = (*e)(i)*gamma*lambda;
                i++;
            }

            for (unsigned _a = 0; _a<this->boardCellNum; _a++) {
                int a = _a;
                if(0) {//if(a!=_action) { // change 2 TODO
                    for (unsigned j=0; j<this->featureSize; j++) {
                        if ((*features)(a,j)==1) {
                            (*e)(j) = 0;
                        }
                    }
                }
            }

            for (unsigned j = 0; j<this->featureSize; j++) {
                if ((*features)(_action,j)) {
                    (*e)(j) += 1.0; //change 1 TODO
                }
                //cout << (*e)(j) << endl;
                //cout.flush();
            }
            //take action in the board.
            generateAfterState(&_gs, __opponent, _action, true, I, &_moves, (_historyOn && I!=E));
            _reward = getReward(&_gs);

            _delta = _reward - (*q)(_action);

            loadF(&_gs);
            loadQ();

            _action = takeAction(&_gs, true);

            if (_reward==0) {//Check if the game is still in progress and that we are not at final goal state.
                _delta += gamma * (*q)(_action);
            }
            _temp  = (alpha)*_delta; //Divide by tilings if necessary
            for (unsigned i =0; i<this->featureSize; i++) {
                (*theta)(i) += _temp * (*e)(i);
            }
            loadQa(_action);


        } while (_reward==0); //Nobody goes home until we get a final game state.
        _historyOn = true;
        if(_reward==10) {
            wins++;
        }
        iterations++;
        _rewardTotal += _reward;

    } while (_reward!=10); //Present the same episode until the agent wins.



    return (double)_rewardTotal/_step;
}

void YAI_SARSA_RL::generateAfterState(GameState * __gs, YAI * __opponent, int __move, bool __this, list<PlayerMove>::reverse_iterator &I, list<PlayerMove> * __history, bool __historyOn) {
    //YGUI  gui;
    if (__this && __move >= 0) { //If not, we dont want to move, only the opponent.
            Coords _temp = __gs->getCoordPair(__move);
            __gs->markCell(_temp.x, _temp.y, currentPlayer);
    }
    Coords _m = __opponent->makeMove(__gs, NULL);
    if (__historyOn && __gs->getCell((*I).x, (*I).y)==EMPTY_CELL) {
        __gs->markCell((*I).x, (*I).y, __opponent->getCurrentPlayer());
        I++;
    }
    else {
        __gs->markCell(_m.x, _m.y, __opponent->getCurrentPlayer());
        if (!__historyOn) {
            __history->push_front(PlayerMove(_m, __opponent->getCurrentPlayer()));
        }
        else {
            I++;
        }
    }
    /*cout << "Q" << endl;
    cout << (*q) << endl;
    cout << "F" << endl;
    cout << (*features) << endl;
    gui.drawBoard(__gs);*/
    cout.flush();

}



double YAI_SARSA_RL::getReward(GameState * __gs) {
    /*if (__gs->isFull()) {
        if (__gs->fullReduce()==currentPlayer) {
            return 10;
        } else {
            return -10;
        }
    }
    return 0;*/
    CellType _result = __gs->fullReduce();
    if (_result==currentPlayer) {
        return 10;
    } else if (_result==EMPTY_CELL) {
        return 0;
    }
    return -10;
}

void YAI_SARSA_RL::loadQ() {
    for (unsigned a=0; a<this->boardCellNum; ++a) {
        (*q)(a) = 0;
        for (unsigned j=0; j<this->featureSize; ++j) {
            if((*features)(a,j)) {
                (*q)(a) += (*theta)(j);
            }
        }
    }
}

/*!
 * \brief This function generates the feature vectors for different actions.
 *
 * At this moment, this function simply copies M times (number of actions)^2 x 3
 * and creates a bit vector of size M for each type of board state (e.g., empty,
 * black, and white), and then it offsets it according to the action. For example, a
 * size 2 board has three groups of 9 cells. Each 9 cell group has three bits for blacks,
 * three for whites, and three for the empty cells.
 *
 * \param __gs The current game state from which features will be extracted.
*/

void YAI_SARSA_RL::loadF(GameState * __gs) {
    for (unsigned i =0; i<this->boardCellNum; i++) {
        for (unsigned j = 0; j<this->boardCellNum; ++j) {
            Coords _c = __gs->getCoordPair(j);
            CellType _t = __gs->getCell(_c.x,_c.y);
            unsigned _offset = (i*this->boardCellNum*3);
            if(_t ==currentPlayer) {
                (*features)(i,j+_offset) = 1;
                (*features)(i,j+(this->boardCellNum)+_offset) = 0;
                (*features)(i,j+_offset+(this->boardCellNum*2)) = 0;
            } else if(_t == EMPTY_CELL) {
                (*features)(i,j+_offset) = 0;
                (*features)(i,j+(this->boardCellNum)+_offset) = 0;
                (*features)(i,j+_offset+(this->boardCellNum*2)) = 1;
            } else {
                (*features)(i,j+(this->boardCellNum)+_offset) = 1;
                (*features)(i,j+_offset) = 0;
                (*features)(i,j+_offset+(this->boardCellNum*2)) = 0;
            }
        }
    }
}

//This function is just a high level cycle that does scaffolding for the operation such as keep track of logs,
//	Mean Square Error (MSE) and other metrics.

bool YAI_SARSA_RL::train() {
    char _buffer[1024];		//Buffer
    double * _averageReward;		  	//Mean Square error
    double * _execTime; 		//Execution time
    double _acc = 0;
    clock_t _start, _end;

    const unsigned _counterMax = 5;
    unsigned _counter = _counterMax;

    //Set the players for the this AI and the opponent.
    this->currentPlayer = this->currentPlayer == EMPTY_CELL ? BLACK_CELL : this->currentPlayer;
    CellType _opponentColour = this->currentPlayer == BLACK_CELL ? WHITE_CELL : BLACK_CELL;

    //Log all the information that we have.
    addLogEntry(STR_INFO_RLAI_LOGSTART, INFO);
    sprintf(_buffer, STR_INFO_RLAI_LOGCONFIG,
        alpha, gamma, epsilon, lambda, boardSize, numIterations);
    addLogEntry(&_buffer[0], INFO);

    //Init boards and opponents.
    int _iter = this->numIterations;
    //Choose the type of opponent depending on the predefined type (opponent). By default, uniform random.
    GameState _gs(this->boardSize);
    YAI * _opponent;
    switch (opponent) {
        //RandomAI
        case 0:
        {
            _opponent = (YAI*)new YAI_Random(_opponentColour) ;
            break;
        }
        //SARSA Lambda, with name = 100
        case 1: {
            _opponent = new YAI_SARSA_RL(_opponentColour, this->boardSize);
            _opponent->setId(100);
            _opponent->deSerializeData();
            assert(_opponent->getBoardSize()==this->boardSize);
            break;
        }
        case 2: {
            _opponent = new YAI_MCTS(_opponentColour, this->boardSize);
            _opponent->setNumThreads(2);
            ((YAI_MCTS*)_opponent)->setPersistence(false);
            //_opponent->setId(100);
            break;
        }
    }

    //Some data to be stored.
    _execTime = new double[1];
    _averageReward = new double[_iter];

    this->setWins(0);
    _start = clock();
    while(_iter) {

        _averageReward[numIterations-_iter] = this->runEpisode(&_gs, _opponent); //Accumulate error.
        _gs.setBoard(this->boardSize);
        if (_counter == 0) {
            _counter = _counterMax;
            addCSVEntry(STR_INFO_RLAI_MSESTEP, avg(&_averageReward[numIterations-_iter-_counterMax], _counterMax),
                    std_err(&_averageReward[numIterations-_iter-_counterMax], _counterMax));
            //cout << "Win ratio at this time: " << 100*((double)(this->wins/(numIterations-_iter+1))) << endl;
            //addCSVEntry("Win Ratio so far", 0, ((double)this->wins)/(_counterMax)*100);
            this->setWins(0);
            _acc = 0;

        } else {
            _counter--;
            _acc += _averageReward[numIterations-_iter];
        }
        _iter--;
    }
    _end = clock();
    _execTime[0] = ((double)((double)(_end-_start))/CLOCKS_PER_SEC);
    addLogEntry(STR_INFO_RLAI_LOGEND, INFO);
    addLogEntry(STR_INFO_RLAI_LOGEND, INFO);
    addCSVEntry(STR_INFO_LOGRUNSTATS, avg(_execTime, 1), std_err(_execTime, 1));
    addCSVEntry(STR_INFO_LOGRUNERROR, avg(_averageReward, numIterations), std_err(_averageReward, numIterations));
    delete []_execTime;
    delete [] _averageReward;
    delete _opponent;
    return true;
}

void YAI_SARSA_RL::loadQa(int __a) {
    if (__a>0) {
        (*q)(__a) = 0;
        for (unsigned j=0; j<this->featureSize; ++j) {
            if ((*features)(__a,j)) {
                (*q)(__a) += (*theta)(j);
            }
        }
    }
    //cout << "Q:" << endl;
    //cout << (*q) <<endl;
    //cout.flush();
}

bool YAI_SARSA_RL::serializeData() {
    std::ofstream ofs(getDataFilename(), std::ios::binary);
        boost::archive::binary_oarchive oa(ofs);
        oa << (*this);
    return true;
}

YAI_SARSA_RL& YAI_SARSA_RL::operator=(const YAI_SARSA_RL& __other ) {
    *this->features = *__other.features;
    this->featureSize = __other.featureSize;
    this->boardSize = __other.boardSize;
    *this->theta = *__other.theta;
    this->lambda = __other.lambda;
    this->gamma = __other.gamma;
    this->epsilon = __other.epsilon;
    //this->alpha = __other.alpha;
    this->id = __other.id;
    this->boardCellNum = __other.boardCellNum;
    this->logging  = __other.logging;
    this->learning = __other.learning;
    *this->q = *__other.q;
    return *this;
}

char * YAI_SARSA_RL::getDataFilename() {
    char * _buffer = new char[128], * _buffer2 = new char[64];
    _buffer[0] = '\0';
    strcat(_buffer, STR_FILENAME_RLAIDATA);
    sprintf(_buffer2, "_%d.dat", this->id);
    strcat(_buffer, _buffer2);
    return _buffer;
}

//Deserializes the data from the binary file and loads it back.

bool YAI_SARSA_RL::deSerializeData() {
    try {
        YAI_SARSA_RL _temp;

        std::ifstream ifs(getDataFilename(), std::ios::binary);
            boost::archive::binary_iarchive ia(ifs);
            ia >> _temp;
        (*this) = _temp;
        _temp.setLogging(false);
        initialized = true;
        return true;
    } catch (exception &e) {
        return false;
    }
}

//Sets a parameter with a new value as specified by the controller.

bool YAI_SARSA_RL::setParams(AIParams __params, double __value) {
    switch(__params) {
        case Iterations:
        {
            char _str[128];
            char _temp[128];
            _str[0] = '\0';
            _temp[0] = '\0';
            int _val = (int)floor(__value);
            strcat(_str, STR_UPDATE_PARAMITER);
            sprintf(_temp,"%d",_val);
            strcat(_str, _temp);
            if(logging) addLogEntry(_str, INFO);
            this->numIterations = _val;
            return true;
            break;
        }
        case Id:
        {
            this-> id = (int)floor(__value);
            return true;
        }
        case BoardSize: //Not really supported
        {
            if(this->boardSize!=(int)floor(__value)) {
                ((int)floor(__value));
            }
            return true;
        }
        case Opponent:
        {
            this->opponent = (int)floor(__value);
            return true;
        }
        default:
            return false;
            break;
    }
}


//Adds a new log entry to the list cache. Note that this data structure is kept in memory
// 	and is flushed only upon request to avoid slowing the learning process with file I/O.
//	This could result in loss of information in case the application crashes, so it makes
//	sense flushing the logCache periodically.

void YAI::addLogEntry(const char * _message, LogEntryType _type) {
    if (logging) {
        LogEntry _entry;
        strcpy(_entry.buffer, _message);
        _entry.type = _type;
        logCache.push_front(_entry);
    }
}

//Analogously to the addLogEntry but for CSV files.

void YAI::addCSVEntry(const char * _message, float _mean, float _std) {
    if (logging) {
        LogCSVEntry _entry;
        strcpy(_entry.buffer, _message);
        _entry.mean = _mean;
        _entry.std = _std;
        logCSVCache.push_front(_entry);
    }
}

//Flushes all the data in the logs from memory to disk.

void YAI::dumpLogs() {
    while (!logCache.empty()) {
        add_log_entry(&logHandle, logCache.back());
        logCache.pop_back();
    }
    while (!logCSVCache.empty()) {
        add_csv_entry(&csvHandle, logCSVCache.back());
        logCSVCache.pop_back();
    }
}

//Attributes to be serialized. WARNING: make changes to the "operator =" if more fiels are added.

template<class Archive>
void YAI_SARSA_RL::serialize(Archive & ar, const unsigned int version) {
    ar & this->featureSize;
    ar & this->boardSize;
    ar & this->theta;
    ar & this->lambda;
    ar & this->gamma;
    ar & this->epsilon;
    ar & this->alpha;
    ar & this->features;
    ar & this->id;
    ar & this->boardCellNum;
    ar & this->logging;
    ar & this->learning;
    ar & this->q;
}

//With a e-greedy probability, select a random action.

bool YAI_SARSA_RL::epsilonGreedy(double _e) {
    return (_e>((double)rand()) / RAND_MAX);
}

void YAI::closeLogs() {
    if (logging) {
        dumpLogs();
        close_csv(&csvHandle);
        close_log(&logHandle);
    }
}

//Resets this object and cleans all data structures.

void YAI_SARSA_RL::reset() {
    if (initialized) {
        delete e;
        delete features;
        delete theta;
        delete q;
    }
    if(initialized) {
        closeLogs();
    }
    this->id = 0;
    this->initialized = false;
    this->featureSize = -1;
    this->boardSize = -1;
    this->boardCellNum = -1;
    this->learning = false;
    this->logCache.clear();
    this->logCSVCache.clear();
}

/*!
 * \brief This function rates all the nodes based on the value function used to determine the move.
 */

list<YAI::MoveScore> YAI_SARSA_RL::assessAllMoves(GameState * __gs) {
    list<MoveScore> _scores;
    if (!initialized) {
        return _scores;
    }

    loadF(__gs);
    loadQ();

    double _min = DBL_MAX, _max = DBL_MIN ;

    for(int i =0; i<this->boardCellNum; ++i) {
        double _val = (*q)(i);
        if (_val < _min) {
            _min = _val;
        }
        if (_val > _max) {
            _max = _val;
        }
    }

    double _range = _max-_min;

    for (int i = 0; i<this->boardCellNum; ++i) {
        Coords _temp = __gs->getCoordPair(i);
        double _val;
        if(__gs->getCell(_temp.x, _temp.y)==EMPTY_CELL) {
            _val = abs(((*q)(i) - _min)/_range);    //a number between zero and one.
        } else
        {//If the cell is taken we definitely don't want to consider it
            _val = 0;
        }
        _scores.push_front(YAI::MoveScore(__gs->getCoordPair(i), i, _val));
    }

    return _scores;
}



/**** CLASS IMPLEMENTATION FOR MCTS AI MANAGER **************************************************************************
 *    For more documentation, see YAI.h
 *
 **/

/*! \brief Initializer module. Builds the basic memory structures and files.
*/

void YAI_MCTS::init() {
    if(logging) {
        initLogs(STR_FILENAME_MCTSLOG, STR_FILENAME_MCTSCSV);
    }
    maxSecondsPerRun = DOUBLE_MCTS_SECSPERMOVE;
    numThreads = sysconf( _SC_NPROCESSORS_ONLN ) / 2; //We don't want to use all the logical CPUs. This could be set in a fancier way... but let's use this number and let the user decide.
    gameInProgress = false;
    persistentTree = true;
}

/*!
 * \brief Destructor. Cleans all dynamic memory structures and purges all logs.
*/

YAI_MCTS::~YAI_MCTS() {
    if (logging) {
        closeLogs();
    }
    //If the nodes are still hanging around, kill'em all.
    if(persistentTree) {
        if(gameInProgress) {
            setOriginalRoots();
            for(unsigned i = 0; i<numThreads; ++i) {
                delete roots[i]->getRaveHistoryPtr();
                roots[i]->harakiri();
                delete roots[i];
            }
            delete [] roots;
        }
    }
}

/*! \brief This function initializes the object and the main datastructures.
*/

void YAI_MCTS::Node::init(YAI_MCTS::Node * __parent, YAI_MCTS * __ai, list<Node> * __raveHist) {
    parent = __parent;
    visits = 0;
    wins = 0;
    workingColour = (__ai) ? __ai->getCurrentPlayer() : EMPTY_CELL;
    state = Leaf;
    gs = NULL;
    children.clear();
    boardId = -1;
    colour = EMPTY_CELL;
    raveHistory = __raveHist;
    this->ai = __ai;
}


/*!
 * \brief This function calculates the RAVE value for a node.
 *
*/


double YAI_MCTS::Node::getRAVE(double _t) {
    bool __uct = true;
    //Nodes labelled as final should not be considered for exploration
    if((this->getState()==Final || this->getState()==TerminalWin || this->getState()==TerminalLoss)) {
        __uct = false;
    }

    //First, let's find the node in the history
    list<Node>::iterator I = this->raveHistory->begin();
    Node * _hist = NULL;
    while(!_hist && this->raveHistory!=NULL) {
        if ((*I).getId()==this->getId() && (*I).getColour()==this->getColour()) {
            _hist = &(*I);
        }
        I++;
    }

    //For the scores functionality: let's bias the children search iff our parent is the current root
    double _scoresBias = 0;
    if (ai->isScoresEnabled() ) {
        int k = 0;
        while(k<ai->getNumThreads() && this->getParent()!=ai->roots[k]) {k++;} //TODO This could be a performance bottleneck
        if (k<ai->getNumThreads()) {
            //We are at the first level
            Coords _coords = this->parent->getGameState()->getCoordPair(this->getId());
            MoveScore _score = MoveScore(_coords, this->getId(), 0);
            //Coords is in a base 10 board. We need to transform it to a base 5. It is not done algorithmically because we don't
            //want to slow down this calculation -- yes, it does look like an ugly mess.
            list<MoveScore>::iterator _findIter = std::find(ai->scores.begin(), ai->scores.end(), _score); //TODO check if this is a contention issue.
            _scoresBias = (*_findIter).getValue();
        }

    }
    double _ntilde = _hist->getVisits();
    double _wtilde = (this->workingColour==ai->getCurrentPlayer()) ? _hist->getWins() : _ntilde - _hist->getWins();

    double _n = this->visits;
    double _w = (this->workingColour==ai->getCurrentPlayer()) ? wins: _n -wins;

    double _c = sqrt(2);

    double _beta = _ntilde / (_n+_ntilde+4*DOUBLE_MCTS_BETA*_n*_ntilde);

    double _temp = (1-_beta)*(_w/_n) + _beta*((_wtilde/_ntilde)*0.90 + _scoresBias*0.10) + _c * sqrt(log(_t)/this->visits);

    //double _temp = (this->workingColour==ai->getCurrentPlayer()) ? ((double)this->wins/this->visits) : (((double)this->visits-this->wins)/this->visits);
    //_temp += sqrt(2) * sqrt(log(_t)/this->visits);
    return _temp; //+ (_scoresBias > 0 ? _scoresBias : 0);
}


//Calculate the UCT value of a node
double YAI_MCTS::Node::getUCT(double _t, bool __uct) {
    //If we have RAVE enabled
    if (raveHistory) {
        return this->getRAVE(_t);
    }
    //Nodes labelled as final should not be considered for exploration
    if((this->getState()==Final || this->getState()==TerminalWin || this->getState()==TerminalLoss)) {
        __uct = false;
    }
    double _temp = (this->workingColour==ai->getCurrentPlayer()) ? ((double)this->wins/this->visits) : (((double)this->visits-this->wins)/this->visits);
    _temp += __uct ? sqrt(2) * sqrt(log(_t)/this->visits) : 0;
    return _temp;
}

YAI_MCTS::Node * YAI_MCTS::Node::getBest(bool _uct) {
    if (this->children.empty()) {
        return NULL;
    }
    else {

        list<Node>::iterator I = children.begin();
        list<Node>::iterator E = children.end();
        list<Node>::iterator M = I;

        //Get t value first
        double _t  = (*I).getVisits();
        I++;
        while(I!=E) {
            _t += (*I).getVisits();
            I++;
        }

        I = children.begin();
        E = children.end();
        unsigned _ties = 1;
        double _max = (*I).getUCT(_t, _uct);
        double _temp;
        I++;
        while (I!=E) {
            _temp = (*I).getUCT(_t, _uct);
            if (_temp >= _max) {
                if (_temp > _max) {
                    M = I;
                    _max = _temp;
                }
                else { //TODO: check this part makes sense.
                    _ties++;
                    if(rand()%_ties==0) {
                        M = I;
                    }
                }
            }
            I++;
        }
        return &(*M);
    }
}

//Run a single simulation all the way down.
bool YAI_MCTS::Node::runOneSimulation(GameState * __gs, YAI * __ourPolicy, YAI * __opponentPolicy, bool _invert) {
    this->visits++;
    list<Coords> _movesToUndo;
    while (!__gs->isFull()) {
        //if (!ai->stillTime()) {
            //throw YException(MCTSTimeOut);
        //}
        Coords _move;
        if(!_invert) {
            _move = __ourPolicy->makeMove(__gs);
            __gs->markCell(_move.x, _move.y, __ourPolicy->getCurrentPlayer());
            _movesToUndo.push_front(_move);
            _move = __opponentPolicy->makeMove(__gs);
            __gs->markCell(_move.x, _move.y, __opponentPolicy->getCurrentPlayer());
            _movesToUndo.push_front(_move);
            //}
        } else {
            _move = __opponentPolicy->makeMove(__gs);
            __gs->markCell(_move.x, _move.y, __opponentPolicy->getCurrentPlayer());
            _movesToUndo.push_front(_move);
            _move = __ourPolicy->makeMove(__gs);
            __gs->markCell(_move.x, _move.y, __ourPolicy->getCurrentPlayer());
            _movesToUndo.push_front(_move);
        }
    }
    bool _win = __gs->fullReduce()==workingColour;
    //bool _win = true;
    for (list<Coords>::iterator I = _movesToUndo.begin(), E = _movesToUndo.end(); I!=E; ++I) {
        __gs->markCell((*I).x, (*I).y, EMPTY_CELL);
    }
    if(this->raveHistory) {
        list<Node>::iterator I = raveHistory->begin(), E =raveHistory->end();
        while(I!=E) {
            Coords _temp = __gs->getCoordPair((*I).getId());
            if ((*I).getColour()!=__gs->getCell(_temp.x, _temp.y)) {
                I++;
                (*I).increaseVisits();
                if (_win) {
                    (*I).increaseWins();
                }
            } else if ((*I).getColour()!=EMPTY_CELL) {
                (*I).increaseVisits();
                if (_win) {
                    (*I).increaseWins();
                }
                I++;
            }
            I++;
        }
    }
    if (_win) {
        this->wins++;
        return true;
    }
    return false;
}



/*!
 * \brief This functions kills all the nodes that are descendants of this particular node (post-order deletion).
*/
void YAI_MCTS::Node::harakiri() {
    if (this->children.empty()) {
        return;
    }
    for(list<Node>::iterator I = this->children.begin(), E = this->children.end(); I!=E; I++) {
        (*I).harakiri();
    }
    this->children.clear();
}

/*!
 * \brief A simple recursive function that changes the colour of all the nodes in the tree in order to be able to use
 * it with different starting colours.
 *
 * Note that this does not change the workingColour as this is only set upon initialization.
 *
*/

void YAI_MCTS::Node::switchColoursRecursive(bool _switchWorkingColour) {
    this->colour = this->colour == WHITE_CELL ? BLACK_CELL : WHITE_CELL;
    if (_switchWorkingColour) this->workingColour = this->workingColour == WHITE_CELL ? BLACK_CELL : WHITE_CELL;
    if (this->children.empty()) {
        return;
    }
    for(list<Node>::iterator I = this->children.begin(), E = this->children.end(); I!=E; I++) {
        (*I).switchColoursRecursive(_switchWorkingColour);
    }

}

//A simply setter and reset for the GS object inside each node
void YAI_MCTS::Node::setGameState(GameState * __gs) {
    gs = __gs;
}

//This function will traverse all the tree until it finds the most promising leaf
YAI_MCTS::RunStats YAI_MCTS::Node::expandChildren(YAI * __ourPolicy, YAI * __opponentPolicy) {


    //See if this node is a leaf or an unknown type
    if (this->state == Leaf) {
        if (gs->isFull()) { //It could be a
            if(gs->fullReduce()==__ourPolicy->getCurrentPlayer()) {
                this->state = TerminalWin;
            }
            else {
                this->state = TerminalLoss;
            }
        }
    }
    //if (!ai->stillTime()) {
        //throw YException(MCTSTimeOut);
    //}
    RunStats _stats(0,0);
    switch(this->getState()) {
        case TerminalWin:
        {
            this->visits++;
            this->wins++;
            return RunStats(1,1);
            break;
        }
        case TerminalLoss:
        {
            this->visits++;
            return RunStats(0,1);
            break;
        }
        case Leaf:
        {
            //Expand all children nodes
            this->state = Unknown;
            list<Coords> _childrenPos = gs->getEmptyCells();
            _stats.wins = 0;
            _stats.visits = _childrenPos.size();
            for(list<Coords>::iterator I = _childrenPos.begin(), E = _childrenPos.end(); I!=E; I++) {
                Node _temp(this, this->ai, this->getRaveHistoryPtr());
                _temp.setId(gs->getAbsPos(*I));
                _temp.setColour(this->colour == WHITE_CELL ? BLACK_CELL : WHITE_CELL);
                _temp.setWorkingColour(this->workingColour);
                _temp.setGameState(this->getGameState());// TODO if something breaks this could be it
                //_temp.setGameState(gs);
                Coords _mark = gs->getCoordPair(_temp.getId());
                //GameState _state(*gs);
                gs->markCell(_mark.x, _mark.y, _temp.getColour());
                //Run a simulation HERE
                if(_temp.runOneSimulation(gs, __ourPolicy, __opponentPolicy, __ourPolicy->getCurrentPlayer()==_temp.getColour() )) {
                    _stats.wins++;
                }
                gs->markCell(_mark.x, _mark.y, EMPTY_CELL);
                _temp.setState(Leaf);
                this->children.push_front(_temp);
                //We need to do this to avoid the pointers deleting themselves because of the objects in the stack
                //GameState * _state = new GameState(*_temp.getGameState());
                //children.front().setGameState(&_state);*/
            }
            this->wins += _stats.wins;
            this->visits += _stats.visits;
            return _stats;
            break;
        }
        case Unknown:
            {
                //RunStats _stats(0,0);
                //Continue going recursively down the tree and then backpropagating up.
                //GameState _state(*__gs);
                //But first, check if all children all leaves
                unsigned _tw =0, _tl = 0, _final = 0, _finalVisits = 0, _finalWins = 0;
                for(list<Node>::iterator I = children.begin(), E = children.end(); I!=E; I++) {
                    NodeState _temp = (*I).getState();
                    _tw += (_temp == TerminalWin) ? 1: 0;
                    _tl += (_temp == TerminalLoss) ? 1: 0;
                    if (_temp == Final) {
                        _final ++;
                        _finalWins += (*I).wins;
                        _finalVisits += (*I).visits;
                    }
                }
                //If all the children are leaves, make this a final node and propagate a probability based on observed wins.
                if ((_tw + _tl + _final) == (children.size())) {
                    this->setState(Final);
                    this->wins = _tw + _finalWins;
                    this->visits = _tw + _tl + _finalVisits;

                    _stats.wins = ((rand()% visits)+1) <= wins ? 1 : 0;
                    _stats.visits = 1;
                    return _stats;
                }
                //Else, look for the best child node to get the information.
                Node * _bestNode = this->getBest(true);
                _bestNode->setGameState(gs);
                Coords _move = gs->getCoordPair(_bestNode->getId());
                gs->markCell(_move.x, _move.y, _bestNode->getColour());
                _stats = _bestNode->expandChildren(__ourPolicy, __opponentPolicy);
                gs->markCell(_move.x, _move.y, EMPTY_CELL);
                this->wins += _stats.wins;
                this->visits += _stats.visits;
                return _stats;
                break;
            }
            case Final:
            {
                //RunStats _stats(0,0);
                _stats.wins = ((rand()% visits)+1) <= wins ? 1 : 0;
                _stats.visits = 1;
                return _stats;
            }
    }


}

//Checks if there are no more nodes to expand
bool YAI_MCTS::Node::checkIfDone() {
    int _tw = 0, _tl= 0, _final = 0;
    for(list<Node>::iterator I = this->children.begin(), E = this->children.end(); I!=E; I++) {
        _tw += ((*I).getState() == Node::TerminalWin) ? 1: 0;
        _tl += ((*I).getState() == Node::TerminalLoss) ? 1: 0;
        _final += ((*I).getState() == Node::Final) ? 1: 0;
    }
    if ((_tl+ _tw + _final) == this->children.size()) {
        return true;
    }
    return false;
}


/*!
 * \brief A helper function that has a loop for each thread to perform the MCTS.
 *
 * \param __root A pointer to the MCTS tree for this thread.
*/

void * YAI_MCTS::auxiliaryMainLoop(Node * __root, unsigned __threadId) {
    //Execute the main loop until we run out of time
    CellType _aiColour = this->currentPlayer == WHITE_CELL ? BLACK_CELL : WHITE_CELL ;
    YAI_Random _self(this->currentPlayer);
    YAI_Random _opponent(_aiColour);

    //Expand and explore the tree while there is time.
    while(stillTime()) {
        //Single thread version
        RunStats _stats = __root->expandChildren(&_self, &_opponent);
        //We need to update all the way up, even if our current root is another node.
        Node * _temp = __root;
        while(_temp->getParent()) {
            _temp = _temp->getParent();
            _temp->setWins(_stats.wins + _temp->getWins());
            _temp->setVisits(_stats.visits + _temp->getVisits());
        }
        //If we can't expand this any more, we are done.
        if (__root->checkIfDone()) {
            break;
        }

    }
    //Pthreads other than main must exit.
    if(__threadId) {
        pthread_exit(NULL);
    }
    return NULL;
}


/*!
 * \brief This function updates the root pointers to the current position in the tree.
 *
 * This function first updates the local move history to reflect the current board position. This is a best
 * effort algorithm, which means that under some circumstances it will not represent/reach the optimal node.
 *
 * \param __gs The current game state.
*/

void YAI_MCTS::updateRoots(GameState * __gs, list<PlayerMove>* __history) {
    this->setOriginalRoots();

    if(!__history) {
        return;
    }


    //If no history, nothing to do: already at the best point.
    if(__history->empty() || (!__history->empty() && __history->back().type == roots[0]->getColour())) {
        //We need to switch the colours of the tree in some cases to be able to use current data structures.

        //This means we need to switch the whole tree to be able to keep using it.
        for (unsigned i =0; i<numThreads; ++i) {
            if (roots[i]->getColour()==currentPlayer || __history->back().type == roots[i]->getColour()) {
                roots[i]->switchColoursRecursive(roots[i]->getColour()==currentPlayer);
                if (persistentTree) {
                for(list<Node>::iterator I = roots[i]->getRaveHistoryPtr()->begin(),
                    E= roots[i]->getRaveHistoryPtr()->end(); I!=E; ++I) {
                    (*I).setColour((*I).getColour() == WHITE_CELL ? BLACK_CELL : WHITE_CELL);
                    (*I).setWorkingColour((*I).getWorkingColour() == WHITE_CELL ? BLACK_CELL : WHITE_CELL);
                }
                }
            }
        }

        if (__history->empty()) {
            return;
        }
    }


    //Else, we need to figure out first how the data structures are laid out.
    for(unsigned i = 0; i<numThreads; i++) {
        Node * _childNode = roots[i];
        list<Node>::iterator _childIter, _childEnd;
        CellType _previousColour = __history->back().type == WHITE_CELL ? BLACK_CELL : WHITE_CELL;
        for(list<PlayerMove>::reverse_iterator I = __history->rbegin(), E = __history->rend(); I!=E; ++I ) {
            //Sanity check -- if there is a problem with the list, throw everything away... otherwise, it would be a headache to
            //deal with this
            _childIter = _childNode->getChildren()->begin();
            _childEnd = _childNode->getChildren()->end();
            //If the colours don't match or don't alternate, end.
            if((_childIter!=_childEnd) && ((*_childIter).getColour()!= (*I).type || (*I).type == _previousColour)) {
                setOriginalRoots();
                for(unsigned j = 0; j<numThreads; ++j) {
                    roots[j]->harakiri();
                    if (persistentTree) delete roots[j]->getRaveHistoryPtr();
                    delete roots[j];
                }
                delete [] roots;
                roots = new Node*[numThreads];
                createRoots(__gs, __history);
                return;
            }
            else {
                _previousColour = (*I).type;
                //Look amongst all the children to see if we can find the move; if not, we end at the current root.
                Coords _tempCoord;
                _tempCoord.x = (*I).x;
                _tempCoord.y = (*I).y;
                int _ind = __gs->getAbsPos(_tempCoord);
                while(_childIter!=_childEnd) {
                    if ((*_childIter).getId() == _ind) {
                        _childNode = &(*_childIter);
                        break;
                    }
                    _childIter++;
                }
                if (_childIter==_childEnd) {
                    //Let's create the node because the tree does not have it.
                    Node _node(_childNode, _childNode->getAIPtr(), _childNode->getRaveHistoryPtr());
                    _node.setColour((*I).type);
                    _node.setGameState(_childNode->getGameState());
                    _node.setWorkingColour(_childNode->getWorkingColour());
                    _node.setId(_ind);
                    _node.setVisits(1);
                    _node.setWins(1);
                    _node.setParent(_childNode);
                    _node.setState(Node::Leaf);
                    _childNode->setState(Node::Unknown);
                    _node.setRaveHistoryPtr(_childNode->getRaveHistoryPtr());
                    _childNode->getChildren()->push_front(_node);
                    _childNode = &(_childNode->getChildren()->front());

                } else {
                    (*_childIter).setGameState(roots[i]->getGameState());
                }
            }
        }
        //Set the new root and update the gamestate.
        roots[i] = _childNode;
        for(list<Node>::iterator I = roots[i]->getChildren()->begin(), E = roots[i]->getChildren()->end(); I!=E; ++I) {
            (*I).setGameState(roots[i]->getGameState());
        }
    }
    //All done.
}

/*!
 * \brief This method creates the roots in case the gameboard already has a history and the trees have not been initialized.
*/

void YAI_MCTS::createRoots (GameState * __gs, list<PlayerMove> * __history) {
    for(unsigned i =0; i<numThreads; ++i) {

        //First, let's create a List that will keep the rave values.
        list<Node> * _rave = NULL;
        //if (persistentTree) {
            _rave = new list<Node>;
            for (unsigned j = 0; j<(this->boardSize*(this->boardSize+1)/2); j++) {
                Node _tempBlack(NULL, NULL, NULL);
                Node _tempWhite(NULL, NULL, NULL);
                _tempBlack.setColour(BLACK_CELL);
                _tempBlack.setId(j);
                _tempWhite.setColour(WHITE_CELL);
                _tempWhite.setId(j);
                _rave->push_back(_tempBlack);
                _rave->push_back(_tempWhite);
            }
        //}

        roots[i] = new Node(NULL, this, _rave); //Only create a root for a new game.
        roots[i]->setColour(this->currentPlayer == WHITE_CELL ? BLACK_CELL : WHITE_CELL);
        roots[i]->setWorkingColour(this->currentPlayer);
        roots[i]->setId(UINT_MAX); //In this case, this is the thread id too.
        if (__history && !__history->empty()) {
            Node * _parent = roots[i];
            roots[i]->setColour(__history->back().type  == WHITE_CELL ? BLACK_CELL : WHITE_CELL);
            //roots[i]->setWorkingColour(roots[i]->getColour());
            for (list<PlayerMove>::reverse_iterator I = __history->rbegin(), E = __history->rend(); I!=E; ++I) {
                Node _child(_parent, this, _rave);
                _child.setColour((*I).type);
                Coords _temp;
                _temp.x = (*I).x;
                _temp.y = (*I).y;
                _child.setId(__gs->getAbsPos(_temp));
                _child.setWorkingColour(_parent->getWorkingColour());
                _child.setGameState(_parent->getGameState());
                _parent->getChildren()->push_front(_child);

                _parent->setState(Node::Unknown);
                _parent = &(_parent->getChildren()->front());
            }
        }
    }
}

/*!
 * \brief This function uses MonteCarlo Tree Search to select the best move available within \sa maxSecondsPerRun.
 *
 * This function relies on uniform random playouts where each free cell in E has a probability
 * 1/|E| to be selected. A number of improvements have been implemented to make the search more
 * efficient:
 *
 * * UCT = The selection heuristic to balance exploitation/exploration.
 * * RAVE = All moves as first heuristic
 * * Parallelism = the program uses all available threads (customizable) to run n concurrent MonteCarlo
 *                  trees and adds up all the results.
 *
 * \param __gs The current game state (game board).
*/

Coords YAI_MCTS::makeMove(GameState * __gs, list<PlayerMove>* __history) {
    if(__gs->isFull()) {
        return Coords(0,0);
    }

    //Debugging remove
    //maxSecondsPerRun = 10;
    //numThreads = 4;
    //Start counting the seconds per move.
    startGlobalClock();

    //Create our data structures, but reuse the roots.
    if(!this->gameInProgress || !this->persistentTree) {
        roots = new Node*[numThreads];
        createRoots(__gs, __history);
        this->gameInProgress = true;
    }

    GameState ** _state = new GameState*[numThreads];
    pthread_t * _threadStruct = (numThreads>1) ? new pthread_t[numThreads-1] : NULL;
    Container ** _containers = new Container*[numThreads];
    //Update the game states
    setOriginalRoots();
    for(unsigned i = 0; i<numThreads; ++i) {
         _state[i] = new GameState(*__gs);
         roots[i]->setGameState(_state[i]);
    }

    updateRoots(__gs, __history);


    //Initialize barrier and the main roots for everybody.

    for(int i = numThreads-1; i >=0; i--) {

        _containers[i] = new Container();
        //Beautiful hack to pass the object reference and the root
        _containers[i]->root = roots[i];
        _containers[i]->ai = this;
        _containers[i]->id = i;
        if(i) {
            pthread_create(&_threadStruct[i-1], NULL, &YAI_MCTS::pthreadHelper, (void *)_containers[i]);
        } else {
            auxiliaryMainLoop(roots[i],0);
        }
    }
    //If there are any remaining threads, join them back.
    for (int i=0; i<(numThreads-1); i++)
    {
        pthread_join(_threadStruct[i], 0);
    }

    //Stop
    stopGlobalClock();

    //Iterator array that moves in a lock step fashion
    list<Node>::iterator * I = new list<Node>::iterator[numThreads];

    //Time to merge everybody together in _roots[0]
    for(int i = 0; i<numThreads; ++i) {
        roots[i]->getChildren()->sort(Node::compare);
        I[i] = roots[i]->getChildren()->begin();
    }

    //
    unsigned _rootWins = 0, _rootVisits = 0;
    double _winRatio =0;
    list<Node>::iterator _best;
    /*for(list<Node>::iterator E = roots[0]->getChildren()->end(); I[0]!=E; I[0]++) {
        unsigned _visits = (*I[0]).getVisits(), _wins = (*I[0]).getWins();
        for(int i = 1; i<numThreads; ++i) {
            _visits += (*I[i]).getVisits();
            _wins += (*I[i]).getWins();
            I[i]++;
        }
        (*I[0]).setWins(_wins);
        (*I[0]).setVisits(_visits);
        _rootWins += _wins;
        _rootVisits += _visits;
    }*/

    //We also want to set the global score list
    scores.clear();

    for(list<Node>::iterator E = roots[0]->getChildren()->end(); I[0]!=E; I[0]++) {
            unsigned _visits = (*I[0]).getVisits(), _wins = (*I[0]).getWins();
            for(int i = 1; i<numThreads; ++i) {
                _visits += (*I[i]).getVisits();
                _wins += (*I[i]).getWins();
                I[i]++;
            }
            //Negamax hack
            double _twins = this->currentPlayer == (*I[0]).getWorkingColour() ? _wins : _visits - _wins;
            double _temp = (double)_twins/_visits;
            if(_temp >= _winRatio) {
                _winRatio = _temp;
                _best = I[0];
            }
            scores.push_front(YAI::MoveScore(__gs->getCoordPair((*I[0]).getId()), (*I[0]).getId(), _wins/_visits));
            _rootWins += _wins;
            _rootVisits += _visits;
    }

    //The free moves must be added too.
    for(list<Coords>::iterator I = __gs->getEmptyCellsPtr()->begin(),
            E = __gs->getEmptyCellsPtr()->end(); I!=E; ++I) {
        scores.push_front(YAI::MoveScore(*I, __gs->getAbsPos((*I).x, (*I).y), 0));
    }

    //Set global probability:
    winningProb = _rootWins/(double)_rootVisits;
    iterations = _rootVisits;

    int _ind = -1;
    //Now that we have unified everything, get the most promising node based on all simulations
    if(roots[0]->checkIfDone()) {
        //If all the leaves are final, then we might need to check which one gives us a win (anyway, we have extra time)
        //This checks all the remaining cells to see which one can make the AI win.
        list<Coords> _empties =  __gs->getEmptyCells();
        for(list<Coords>::iterator I =_empties.begin(), E = _empties.end(); I!=E; I++ ) {
            Coords _mark = (*I);
            __gs->markCell(_mark.x, _mark.y, this->currentPlayer);
            if (__gs->fullReduce() == this->currentPlayer) {
                _ind = __gs->getAbsPos(_mark);
            }
            __gs->markCell(_mark.x, _mark.y, EMPTY_CELL);
        }
    }
    if(_ind==-1){
        //Get the best according to the simulation
        //_ind = *(*roots[0]->getBest(false));
        _ind = *(*_best);
    }
    Coords _result = __gs->getCoordPair(_ind);
    //Clean everything up except for the roots that we might need later on.
    for(int i = 0; i<numThreads; ++i) {
        delete _state[i];
        delete _containers[i];
    }

    if (numThreads >1) delete [] _threadStruct;
    delete [] _state;
    delete [] _containers;

    //Delete the tree if we are not using this mode.
    if(!this->persistentTree) {
        setOriginalRoots();
        for(unsigned i =0; i<numThreads; i++) {
            roots[i]->harakiri();
            delete roots[i];
        }
        delete [] roots;
    }

    return _result;
}


void YAI_MCTS::startGlobalClock() {
    running = true;
    clock_gettime(CLOCK_MONOTONIC, &startTime);
}

double YAI_MCTS::stopGlobalClock() {

    struct timespec _stop;
    if(running) {
        running = false;
        clock_gettime(CLOCK_MONOTONIC, &_stop);

    }
    return getElapsedSeconds(startTime, _stop);
}

double YAI::getElapsedSeconds(struct timespec __start, struct timespec __finish){

    double _elapsed;
    _elapsed = (__finish.tv_sec - __start.tv_sec);
    _elapsed += (__finish.tv_nsec - __start.tv_nsec) / 1000000000.0;
    return _elapsed;

}
//Checks if there is still time left for the simluation

bool YAI_MCTS::stillTime() {
    struct timespec _now;

    if(running) {
        //running = false;
        clock_gettime(CLOCK_MONOTONIC, &_now);
        return (maxSecondsPerRun > getElapsedSeconds(startTime, _now));
    }
    return false;
}

/*! \brief A simple method to make the node pointers go all the way up in the tree.
 **/

void YAI_MCTS::setOriginalRoots() {
    for (unsigned i = 0; i<numThreads; ++i) {
        while(roots[i]->getParent()) {
            roots[i] = roots[i]->getParent();
        }
    }
}


/*!
 * \brief This function sets parameters via the interface; however, depending on the parameters
 * certain actions can be taken as well.
 *
 * For example, if the board size is set to a different board size than the one currently used, we delete it.
 * See documentation below for more details.
 *
 * \param __params The type of \sa YAI::AIParams to be updated.
 * \param __value The new value.
 * \return A flag indicating whether the operation was successful or not.
 */

bool YAI_MCTS::setParams(AIParams __params, double __value) {
    switch (__params) {
        case BoardSize:
        {
            unsigned _temp  = (unsigned)floor(__value);
            if(this->gameInProgress && _temp != boardSize) {
                //If there is a game in progress, we need to reset our data structures, particularly if the boardsize has changed.
                setOriginalRoots();
                for(unsigned i= 0; i<numThreads; ++i) {
                    roots[i]->harakiri();
                    if (this->persistentTree) delete roots[i]->getRaveHistoryPtr();
                    delete roots[i];
                }
                delete [] roots;
                roots = NULL;
                 this->gameInProgress = false;
            }
            else if (gameInProgress){   //We can reuse the information.
                this->setOriginalRoots();
            }
            this->boardSize = _temp;
            return true;
            break;
        }
        case ExecutionTime:
            this->maxSecondsPerRun = __value;
            return true;
        case Cores:
        {
            //We cannot allow a change of cores during the game.
            if(!this->gameInProgress) {
                this->numThreads = (unsigned)floor(__value);
                return true;
            }
            else {
                return false;
            }
        default:
            return false;
        }
        case YAI::Mode:
        {
            bool _superMCTS = (int)floor(__value);
            this->persistentTree = _superMCTS;
            return true;
            break;
        }
    }
    return false;
}

bool YAI_MCTS::train() {
    //Implementing the abstract method--however, MCTS won't do anything.
    return true;
}

//Returns miscelleanous information to the UI

YAI::RunInfo YAI_MCTS::getRunInfo() {
    RunInfo _info;
    _info.setValid(true);
    char _buffer[128];
    _info.setIterations(this->iterations);
    sprintf(&_buffer[0], STR_INFO_MCTS_STATS, this->iterations, this->numThreads);
    _info.setMessage(&_buffer[0]);
    return _info;
}

//****************** CLASS YAI_MTMCTS **********************************

bool YAI_MLMCTS::init(unsigned __boardSize) {
    this->boardSize = __boardSize;
    if(initialized) {
        delete aiHighDimension;
        delete aiLowDimension;
    }
    this->initialized  = true;
    this->aiHighDimension = new YAI_MCTS(this->currentPlayer, __boardSize);
    this->aiLowDimension = new YAI_SARSA_RL(this->currentPlayer, __boardSize);
    numThreads = sysconf( _SC_NPROCESSORS_ONLN ) / 2; //We don't want to use all the logical CPUs
    this->aiHighDimension->setNumThreads(numThreads);
    this->aiLowDimension->setNumThreads(numThreads);
    this->aiLowDimension->setId(__boardSize);
    if(!this->aiLowDimension->deSerializeData()) {
        throw YException(CommandInvalid);
    }
}

YAI_MLMCTS::~YAI_MLMCTS() {
    delete this->aiHighDimension;
    delete this->aiLowDimension;
}

/*!
 * \brief YAI_MLMCTS::makeMove This function computes a move for the Multilevel MCTS program.
 * \param __gs The current game state.
 * \param __history The game playing history -- must be legal!
 * \return A set of coordinates that represent the move.
 */

Coords YAI_MLMCTS::makeMove(GameState * __gs, list<PlayerMove>* __history) {
    if(__gs->getBoardSize()!=this->boardSize) {
        throw YException(CommandInvalid);
    }

    Coords _move(0,0);

    if(__gs->isFull()) {
        return _move;
    }
    //YGUI ui;
    //ui.drawBoard(__gs);
    //GameState * _reductionLevel5 = __gs->reduceTo5();
    //ui.drawBoard(_reductionLevel5);
    //return Coords(0,0);
    this->iterations = 0;

    //Set everybody to the correct settings
    //this->aiLowDimension->setNumThreads(this->numThreads);
    this->aiLowDimension->setSecondsPerRun(this->getSecondsPerRun());
    this->aiLowDimension->setCurrentPlayer(this->currentPlayer);

    //this->aiHighDimension->setNumThreads(this->numThreads);
    this->aiHighDimension->setSecondsPerRun(this->getSecondsPerRun());
    this->aiHighDimension->setCurrentPlayer(this->getCurrentPlayer());

    this->aiLowDimension->makeMove(__gs, __history);
    list<MoveScore> _scores = this->aiLowDimension->assessAllMoves(__gs);

    _scores.sort(MoveScore::compare);

    //Enable the scores and send them over.
    this->aiHighDimension->setScores(_scores);
    this->aiHighDimension->setScoresEnabled(true);

    _move = this->aiHighDimension->makeMove(__gs, __history);
    this->iterations = this->aiHighDimension->getRunInfo().getIterations();

    //delete _reductionLevel5;

    //Return the move
    return _move;
}


bool YAI_MLMCTS::setParams(YAI_MLMCTS::AIParams __params, double __value) {
    switch(__params) {
        case YAI::BoardSize:
            //This functionalit has been disabled for this node. Only size 10 boards supported.
            return init((unsigned)floor(__value));

            break;
        case YAI::Cores:
            if (aiLowDimension->setParams(__params, __value) && aiHighDimension->setParams(__params, __value)) {
                this->numThreads = (unsigned)floor(__value);
                return true;
            }
            return false;
            break;
        case YAI::ExecutionTime:
            this->maxSecondsPerRun = __value;
            return false;
            break;
    }
    return false;
}

YAI::RunInfo YAI_MLMCTS::getRunInfo() {
    RunInfo _info;
    _info.setValid(true);
    char _buffer[128];
    _info.setIterations(this->iterations);
    sprintf(&_buffer[0], STR_INFO_MLMCTS_STATS, this->iterations, this->numThreads);
    _info.setMessage(&_buffer[0]);
    return _info;
}
