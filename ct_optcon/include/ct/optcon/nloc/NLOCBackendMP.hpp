/**********************************************************************************************************************
This file is part of the Control Toobox (https://adrlab.bitbucket.io/ct), copyright by ETH Zurich, Google Inc.
Authors:  Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo, Farbod Farshidian
Lincensed under Apache2 license (see LICENSE file in main directory)
**********************************************************************************************************************/


#pragma once

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "NLOCBackendBase.hpp"
#include <ct/optcon/solver/NLOptConSettings.hpp>

namespace ct {
namespace optcon {


/*!
 * NLOC Backend for the multi-threaded case
 */
template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR = double>
class NLOCBackendMP : public NLOCBackendBase<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR>
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef NLOCBackendBase<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR> Base;

    NLOCBackendMP(const OptConProblem<STATE_DIM, CONTROL_DIM, SCALAR>& optConProblem, const NLOptConSettings& settings);

    NLOCBackendMP(const OptConProblem<STATE_DIM, CONTROL_DIM, SCALAR>& optConProblem,
        const std::string& settingsFile,
        bool verbose = true,
        const std::string& ns = "alg");

    //! destructor
    virtual ~NLOCBackendMP();

protected:
    virtual void computeLinearizedDynamicsAroundTrajectory(size_t firstIndex, size_t lastIndex) override;

    virtual void computeQuadraticCostsAroundTrajectory(size_t firstIndex, size_t lastIndex) override;

    virtual void rolloutShots(size_t firstIndex, size_t lastIndex) override;

    SCALAR performLineSearch() override;

private:
    enum WORKER_STATE
    {
        IDLE,
        LINE_SEARCH,
        ROLLOUT_SHOTS,
        LINEARIZE_DYNAMICS,
        COMPUTE_COST,
        PARALLEL_BACKWARD_PASS,
        SHUTDOWN
    };

    void startupRoutine();

    void shutdownRoutine();

    //! Launch all worker thread
    /*!
	  Initializes and launches all worker threads
	 */
    void launchWorkerThreads();

    //! Main function of thread worker
    /*!
	  Includes all tasks that a worker will execute
	 */
    void threadWork(size_t threadId);

    //! Line search for new controller using multi-threading
    /*!
	  Line searches for the best controller in update direction. If line search is disabled, it just takes the suggested update step.
	 */
    void lineSearchWorker(size_t threadId);


    //! Worker function for linearized dynamics
    /*!
	  Gets a parameter k to process and then calls computeLinearizedDynamicsWorker method
	  \param k step k
	 */
    void computeLinearizedDynamicsWorker(size_t threadId);


    //! Computes the quadratic costs
    /*!
	  This function calculates the quadratic costs as provided by the costFunction pointer.

	  \param k step k
	 */
    void computeQuadraticCostsWorker(size_t threadId);


    //! rolls out a shot and computes the defect
    void rolloutShotWorker(size_t threadId);

    //! Creates the linear quadratic problem
    /*!
	  This function calculates the quadratic costs as provided by the costFunction pointer as well as the linearized dynamics.

	  \param k step k
	 */
    void computeLQProblemWorker(size_t threadId);


    /*! heuristic that generates a unique id for a process, such that we can manage the tasks.
	 * Generates a unique identifiers for task, iteration:
	 * @todo replace by proper hash
	 * */
    size_t generateUniqueProcessID(const size_t& iterateNo, const int workerState);

    //! wrapper method for nice debug printing
    void printString(const std::string& text);

    std::vector<std::thread, Eigen::aligned_allocator<std::thread>> workerThreads_;
    std::atomic_bool workersActive_;
    std::atomic_int workerTask_;

    std::mutex workerWakeUpMutex_;
    std::condition_variable workerWakeUpCondition_;

    std::mutex kCompletedMutex_;
    std::condition_variable kCompletedCondition_;

    std::mutex kCompletedMutexCost_;
    std::condition_variable kCompletedConditionCost_;

    std::mutex lineSearchResultMutex_;
    std::mutex alphaBestFoundMutex_;
    std::condition_variable alphaBestFoundCondition_;

    std::atomic_size_t alphaTaken_;
    size_t alphaMax_;
    size_t alphaExpBest_;
    size_t alphaExpMax_;
    std::atomic_bool alphaBestFound_;
    std::vector<size_t> alphaProcessed_;

    std::atomic_size_t kTaken_;
    std::atomic_size_t kCompleted_;

    size_t KMax_;
    size_t KMin_;

    SCALAR lowestCostPrevious_;
};


}  // namespace optcon
}  // namespace ct
