//
// Created by gufe on 4/9/21.
//

#ifndef SVO_IMU_INTEGRATION_H
#define SVO_IMU_INTEGRATION_H
#include <gtsam/navigation/CombinedImuFactor.h>
#include <gtsam/navigation/ImuFactor.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/LevenbergMarquardtOptimizer.h>
#include <gtsam/navigation/ImuBias.h>
#include <gtsam/inference/Symbol.h>
#include <gtsam/slam/ProjectionFactor.h>
#include <gtsam/nonlinear/ISAM2.h>
#include <sophus/se3.h>
#include <svo/frame.h>

using gtsam::symbol_shorthand::B;  // Bias  (ax,ay,az,gx,gy,gz)
using gtsam::symbol_shorthand::V;  // Vel   (xdot,ydot,zdot)
using gtsam::symbol_shorthand::P;  // Pose3 (x,y,z,r,p,y)
using gtsam::symbol_shorthand::C;  // Camera pose

//https://stackoverflow.com/questions/24881799/get-index-in-c11-foreach-loop
// Wrapper class
template <typename T>
class enumerate_impl
{
public:
    // The return value of the operator* of the iterator, this
    // is what you will get inside of the for loop
    struct item
    {
        size_t index;
        typename T::value_type & item;
    };
    typedef item value_type;

    // Custom iterator with minimal interface
    struct iterator
    {
        iterator(typename T::iterator _it, size_t counter=0) :
                it(_it), counter(counter)
        {}

        iterator operator++()
        {
            return iterator(++it, ++counter);
        }

        bool operator!=(iterator other)
        {
            return it != other.it;
        }

        typename T::iterator::value_type item()
        {
            return *it;
        }

        value_type operator*()
        {
            return value_type{counter, *it};
        }

        size_t index()
        {
            return counter;
        }

    private:
        typename T::iterator it;
        size_t counter;
    };

    enumerate_impl(T & t) : container(t) {}

    iterator begin()
    {
        return iterator(container.begin());
    }

    iterator end()
    {
        return iterator(container.end());
    }

private:
    T & container;
};

// A templated free function allows you to create the wrapper class
// conveniently
template <typename T>
enumerate_impl<T> enumerate(T & t)
{
    return enumerate_impl<T>(t);
}

class Imu_Integration{
public:
    Imu_Integration(Sophus::SE3& SE_init);
    ~Imu_Integration();
    bool reset(gtsam::ISAM2& optimizer,boost::shared_ptr<svo::Frame>& new_frame);
    bool reset(gtsam::LevenbergMarquardtOptimizer& optimizer,boost::shared_ptr<svo::Frame>& new_frame);
    bool update(double* imu= nullptr);
    bool predict(boost::shared_ptr<svo::Frame>&,std::size_t&,const double reproj_thresh);
private:
    std::shared_ptr<gtsam::NavState> statePtr;
    std::shared_ptr<gtsam::imuBias::ConstantBias> imu_biasPtr;
    std::shared_ptr<gtsam::Values> valuesPtr;
    std::shared_ptr<gtsam::NonlinearFactorGraph> graphPtr;
    boost::shared_ptr<gtsam::PreintegratedCombinedMeasurements::Params> parameterPtr;
    std::shared_ptr<gtsam::PreintegratedCombinedMeasurements> preintegratedPtr;
    boost::shared_ptr<gtsam::noiseModel::Isotropic> ProjectNoisePtr;
    std::chrono::steady_clock::time_point t_1;
    std::uint32_t imu_factor_id=0;
    uint imu_n=0;
    std::shared_ptr<gtsam::ISAM2Params> optimizerParamPtr;
    bool syn=false;
};
#endif //SVO_IMU_INTEGRATION_H
