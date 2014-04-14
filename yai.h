/** =========================================================
  * 
  * University of Alberta
  * Department of Computer Science
  * CMPUT 670 - Games, graphs and algorithms
  * 
  * Author:	Alejandro Ramirez Sanabria 
		(ramirezs@ualberta.ca)
  * Date:	30/JAN/2014
  * 
  * Abstract:	Assignment 2 - This file defines a basic
  *		abstract class - interface for different 
  *		AI players/heuristics.
  * ========================================================= 
*/ 

#ifndef Y_AI_H
#define Y_AI_H

#include "ygamestate.h"
#include <list>
#include <ctime>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <cstdlib>
#include <fstream>
#include <assert.h>
#include "../../../ARS/logger/logger.h"
#include "../../../ARS/math/math.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <float.h>


namespace ARS {

#define numFeatures 10 //This needs to be revisited.

/** CLASS MODEL 
  * 
  * This class defines an interface to be implemented by 
  * different AI players in order to make them interchangeable. 
**/
	class YAI {
    public:
        class MoveScore;

		protected:
			CellType currentPlayer;
            unsigned boardSize, id, numThreads, iterations;
            double maxSecondsPerRun;
            bool logging;
            bool scoresEnabled;

            list<MoveScore> scores;

            LogHandler logHandle, csvHandle;

            std::list<LogEntry> logCache;
            std::list<LogCSVEntry> logCSVCache;

            //Logging
            virtual void addLogEntry(const char *, LogEntryType);
            virtual void addCSVEntry(const char *, float, float);
            virtual void dumpLogs();
            virtual void initLogs(const char * _logPath, const char * _csvPath);
            virtual void closeLogs();


	
		public:
            //Class for miscellaneous communication with the interface.
            class RunInfo {
                    private:
                        bool valid;
                        unsigned iterations;
                        char buffer[128];
                    public:
                        RunInfo() : valid(false)    {}
                        unsigned getIterations()            {return this->iterations; }
                        void setIterations(unsigned _i)     {this->iterations = _i;}
                        void setValid(bool _v)              {this->valid = _v;}
                        bool isValid()                      {return this->valid;}
                        void setMessage(const char * _msg)  {strcpy(&buffer[0], _msg);}
                        const char * getMessage()           {return &buffer[0];}

            };

            //Class for assessing information supported by some AI agents.
            class MoveScore {
                private :
                    Coords coords;
                    unsigned id;
                    double value;
                public:
                    MoveScore(Coords _c, unsigned _id, double _value) : coords(_c), id(_id), value(_value) {}
                    void setCoords(Coords _c, unsigned _id)    {  this->coords = _c; this->id = _id; }
                    void setValue(double _val)  {  this->value = _val;}
                    Coords getCoords()      { return this->coords; }
                    double getValue()       { return this->value; }
                    unsigned getId()        { return this->id;}
                    static bool compare(const MoveScore & _n1, const MoveScore & _n2) { return (_n1.id < _n2.id);}
                    bool operator==(const MoveScore &__other) const {return (this->coords == __other.coords && this->id == __other.id);}
                    bool operator!=(const MoveScore &__other) const {return !((*this)==__other);}
            };

            //Information
            virtual RunInfo getRunInfo() { RunInfo _t; return _t; }

            static double getElapsedSeconds  (struct timespec, struct timespec);

            enum AIParams {Iterations, Id, BoardSize, Opponent, ExecutionTime, Cores, Mode};
            YAI() : currentPlayer(BLACK_CELL), boardSize(INT_DEFAULT_BOARDSIZE), logging(false),
                numThreads(INT_DEFAULT_NUMCORES), maxSecondsPerRun(DOUBLE_MCTS_SECSPERMOVE), scoresEnabled(false),
                    iterations(0) {}
            YAI(CellType _t): currentPlayer(_t), boardSize(INT_DEFAULT_BOARDSIZE), logging(false),
                              numThreads(INT_DEFAULT_NUMCORES), maxSecondsPerRun(DOUBLE_MCTS_SECSPERMOVE), scoresEnabled(false),
                              iterations(0) {}
            YAI(CellType _t, unsigned _bs): currentPlayer(_t), boardSize(_bs), logging(false),
                                            numThreads(INT_DEFAULT_NUMCORES), maxSecondsPerRun(DOUBLE_MCTS_SECSPERMOVE), scoresEnabled(false),
                                            iterations(0) {}
            virtual ~YAI() {}

			void setCurrentPlayer (CellType _t) {currentPlayer = _t;}
			CellType getCurrentPlayer () {return currentPlayer; }

            virtual list<MoveScore> assessAllMoves(GameState * _gs) { return scores;}


            virtual Coords makeMove(GameState *, list<PlayerMove>* _l = NULL) = 0;
			
			virtual bool setParams(AIParams, double _value) = 0;
            virtual bool train() = 0;
            double testAgainst(YAI *, unsigned) ;
			
			virtual bool serializeData() = 0;
			virtual bool deSerializeData() = 0; 

            bool loggingEnabled()       { return logging; }
            void setLogging(bool _l) 	{ this->logging = _l; }

            int getId() {return id;}
            void setId(int __i) {id = __i;}

            void setNumThreads(unsigned _t)     { this->numThreads = _t;}
            unsigned getNumThreads()            { return this->numThreads; }

            virtual void setBoardSize(unsigned _t)      { this->boardSize= _t;}
            virtual unsigned getBoardSize()             { return this->boardSize; }

            double getSecondsPerRun()           { return this->maxSecondsPerRun; }
            void setSecondsPerRun(double _s)    { this->maxSecondsPerRun = _s;}

            unsigned getIterations()            { return iterations;}
            void setIterations(unsigned __i)    { this->iterations = __i;}

            void setScores(list<MoveScore> __scores) {
                scores.clear();
                this->scores.insert(this->scores.end(), __scores.begin(), __scores.end());
            }

            void setScoresEnabled(bool _t)      { this->scoresEnabled = _t; }
            bool isScoresEnabled()              { return this->scoresEnabled;}

	};


/** CLASS MODEL 
  * 
  * This class defines a random implementation of the YAI interface. 
  * It works by simply taking a random position amongst the free cells
  * in the board. 
**/
	class YAI_Random : public YAI {		
		public:
			YAI_Random() {}
			~YAI_Random() {}
			YAI_Random(CellType _t) : YAI(_t) {}
            YAI_Random(CellType _t, unsigned _bs) : YAI(_t, _bs) {}
            Coords makeMove(GameState *, list<PlayerMove>* _p =NULL);
			bool train() { return true; } 				//No training required
			
            bool setParams(AIParams, double);
			
			bool serializeData() 	{ return false; }
			bool deSerializeData() 	{ return false; }
	};	

/** CLASS MODEL 
  *
  * TODO
**/

	class YAI_SARSA_RL : public YAI {
        typedef boost::numeric::ublas::vector<double> Vector;
        typedef boost::numeric::ublas::matrix<double> Matrix;
		
		public:
        //Constructors/destructors
			YAI_SARSA_RL() : initialized(false) {init();} 
			YAI_SARSA_RL(CellType __t): YAI(__t), initialized(false) {init();}
            YAI_SARSA_RL(CellType __t, unsigned __bs = INT_DEFAULT_BOARDSIZE)
                    : YAI(__t,__bs), lambda(DOUBLE_RLAI_LAMBDA), gamma(DOUBLE_RLAI_GAMMA), alpha(DOUBLE_RLAI_ALPHA), epsilon(DOUBLE_RLAI_EPSILON),
                    learning(true), initialized(false)
					{ init(__bs); }
			YAI_SARSA_RL(CellType __t, unsigned __bs, double __alpha, double __gamma, double __lambda, double __epsilon, bool _l, bool _log) 
                    : YAI(__t), lambda(__lambda), gamma(__gamma), alpha(__alpha), epsilon(__epsilon), learning(_l), initialized(false)
					{ init(__bs); }
			~YAI_SARSA_RL();
		//Other public methods
            Coords makeMove(GameState *, list<PlayerMove>* _p = NULL);
			bool setParams(AIParams, double);

            int getWins() {return wins;}
            void setWins(int __i) {wins = __i;}

            bool getLearning()          { return learning; }
            void setLearning(bool _l) 	{ this->learning = _l; }

			bool train();

            void setBoardSize(unsigned _t)  { init(_t); }

			double runEpisode(GameState *, YAI *);	
			bool serializeData();
			bool deSerializeData();
            YAI_SARSA_RL& operator=(const YAI_SARSA_RL&);
            list<MoveScore> assessAllMoves(GameState * _gs);

		private:

		//Attributes


            Vector *e, *q, *theta;
			Matrix *features;
            int wins;
            int opponent;
			unsigned numIterations; 
            unsigned featureSize, boardCellNum;
			double lambda, gamma, alpha, epsilon;  
            bool learning, initialized;

		//Other methods
		 

			bool init() {return init(INT_DEFAULT_BOARDSIZE);}
			bool init(unsigned); 
			void reset();
			void loadQ();
            void loadQa(int);
            void loadF(GameState *);
			bool 	epsilonGreedy(double);  
			double 	getReward(GameState *);
            void 	generateAfterState(GameState *, YAI*, int,  bool, list<PlayerMove>::reverse_iterator &, list<PlayerMove> *, bool);
			int  	takeAction(GameState *, bool);
		//Serialization	
			char * getDataFilename(); 	
			friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version);
	};
    /** CLASS MODEL
     *
     *  This is a naive, time bouand and threaded monte carlo tree search implementation.
     *
    */
    class YAI_MCTS : public YAI {

    private:
        //Struct for run stat information
        typedef struct RunStats {
            unsigned wins;
            unsigned visits;
            RunStats(unsigned _n, unsigned _m) : wins(_n), visits(_m) {}
        } RunStats;


        //Nodes for the MCTS tree. Note that these nodes are pametrizable

        class Node {
        public:
            enum NodeState {TerminalWin, TerminalLoss, Leaf, Unknown, Final};
        private:
                //All the different elements of each node. The parent has a list with all the children.
                Node * parent;
                unsigned visits;
                unsigned wins;
                NodeState state;
                list<Node> children;
                unsigned boardId;
                YAI_MCTS * ai;
                GameState * gs;
                CellType colour;
                CellType workingColour;
                list<Node> * raveHistory;

                //Initialization
                void init(Node * _parent=  NULL, YAI_MCTS * _ai = NULL, list<Node> * _raveHist = NULL);

        public:

                //Compare
                static bool compare(const Node & _n1, const Node & _n2) { return (*_n1 < *_n2);}
                //Constructors
                Node()          { init(); }
                Node(Node * _p) { init(_p);}
                Node(Node * _p, YAI_MCTS * _ai, list<Node> * _raveHist)     { init(_p, _ai, _raveHist);}

                ~Node()                             { }

                //Setters and getters
                unsigned getId() const          { return boardId; }
                void setId(unsigned __u)        { this->boardId=__u;}
                unsigned getVisits() const      { return visits; }
                void increaseVisits()           { this->visits++; }
                void increaseWins()             { this->wins++; }
                void setVisits(unsigned __u)    { this->visits=__u;}
                unsigned getWins() const        { return wins; }
                void setWins(unsigned __u)      { this->wins=__u;}
                Node * getParent() const        { return this->parent;}
                void setParent(Node * _p)       { this->parent= _p;}
                NodeState getState () const     { return this->state; }
                void setState (NodeState _s)    { this->state=_s;}
                unsigned operator*( ) const     { return this->getId();}
                void setColour (CellType _p)    { this->colour = _p; }
                CellType getColour() const      { return this->colour; }
                void setWorkingColour (CellType _p)    { this->workingColour = _p; }
                CellType getWorkingColour() const      { return this->workingColour; }
                YAI_MCTS * getAIPtr()           { return this->ai;}
                list<Node> * getRaveHistoryPtr()       { return this->raveHistory; }
                void setRaveHistoryPtr(list<Node> * _p){ this->raveHistory = _p; }

                //Checks if we are done
                bool checkIfDone();

                //GameState
                GameState * getGameState() const        { return this->gs; }
                void setGameState (GameState *);

                //Children operators.
                list<Node> * getChildren ()             { return &this->children; }
                RunStats expandChildren(YAI * , YAI * ) ;

                //UCT
                double getUCT(double, bool);

                //RAVE
                double getRAVE(double);

                //Simulation
                bool runOneSimulation(GameState *, YAI *, YAI *, bool);

                //Selection
                Node * getBest (bool);

                //Harakiri >:D
                void harakiri();
                void switchColoursRecursive(bool);

                //Reset
                void reset(Node * _p, YAI_MCTS * _ai) { init(_p, _ai);}

        };

            //! A collection of different MCTS trees, one per thread.
            Node ** roots;

            double winningProb;
            bool gameInProgress;
            bool persistentTree;
            bool running;
            struct timespec startTime;

        //Public methods
        public:

            //Constructors and destructors.
            YAI_MCTS() { init(); }
            YAI_MCTS(CellType __t, unsigned __bs ) : YAI(__t, __bs) { init(); }
            ~YAI_MCTS();

            //Abstract methods tthat need to be implemented.
            Coords makeMove(GameState *, list<PlayerMove>*);

            bool setParams(AIParams, double _value);
            bool train();

            //Serializers and deserializers -- not used at the moment.
            virtual bool serializeData()        { return false; }
            virtual bool deSerializeData()      { return false; }

            void setOriginalRoots();

            void createRoots(GameState *, list<PlayerMove>*);
            void updateRoots(GameState *, list<PlayerMove> *);

            //Settes and getters
            RunInfo getRunInfo();

            void setPersistence(bool _t)    { this->persistentTree = _t; }
            bool isPersistent()             { return this->persistentTree; }

            /*! \class Container
             *  \brief A simple class that allows inter-thread (pthread) communication across the object.
             **/
            class Container {
                friend class YAI_MCTS;
                private :
                YAI_MCTS * ai;
                Node * root;
                unsigned id;
            };
    private:
            friend class YAI_MCTS::Node;
            //Class initializer
            void init();
            void * auxiliaryMainLoop(Node*,unsigned);

            //Clock

            bool stillTime();
            void startGlobalClock();
            double stopGlobalClock();


            //Helper for pthreads
            static void *pthreadHelper(void *context)
            {
                Container * _cont = (Container *)context;
                return _cont->ai->auxiliaryMainLoop(_cont->root, _cont->id);

            }

    };

    /** CLASS MODEL : Multi-level multi-threaded MCTS for HEX
     *  ======================================================
     *  Author: Alejandro Ramirez
     *  Email:  ramirezs@ualberta.ca
     *
     *  Class Description:
     *
     *  This class uses Y-reduction to operate MCTS at different abstraction levels.
     *  The default granularity is 5, which means that any n-board would be reduced to a
     *  5x5 board (if boardSize < 5, a simple threaded naive MCTS is used given its triviality).
     *
     *  STEP 1
     *  +----------+        +----------+
     *  | n - dim  | ====>  |  5 - dim |
     *  +----------+        +----------+
     *
     * STEP 2: Solver 1 on the 5- dim space.
     *
     *  STEP 3: Increase dimension and focus on the subarea determined on step 2. For each level, if total time
     *  allocated for processing is t seconds, and 1 reduction + x expansions must be performed, then each time t_i
     *  must be a fraction of t, s.t. Sum over i, t_i  = 1.
**/

    class YAI_MLMCTS : public YAI {
    //Class attributes
    private:
        //Different AIs for the higher and lower dimensions
        YAI * aiHighDimension, * aiLowDimension;
        bool initialized;
    //Methods
    public:
        //Constructors/destructors
        YAI_MLMCTS() : initialized(false), YAI() { init(INT_DEFAULT_BOARDSIZE); }
        YAI_MLMCTS(CellType _t, unsigned _bs) : initialized(false), YAI(_t, _bs)    { init(_bs); }
        ~YAI_MLMCTS();

        //Abstract methods tthat need to be implemented.
        Coords makeMove(GameState *, list<PlayerMove>* _p = NULL);

        bool setParams(AIParams, double _value);

        //No training needed.
        bool train()                        { return true; }


        //No serialization needed
        virtual bool serializeData()        { return false; }
        virtual bool deSerializeData()      { return false; }

        //Run information and stats
        RunInfo getRunInfo();

    private:
        bool init(unsigned int); //Class initializer

    };

}

#endif
