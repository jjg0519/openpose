#ifndef OPENPOSE_FACE_W_FACE_RENDERER_HPP
#define OPENPOSE_FACE_W_FACE_RENDERER_HPP

#include <memory> // std::shared_ptr
#include <openpose/thread/worker.hpp>
#include "faceRenderer.hpp"

namespace op
{
    template<typename TDatums>
    class WFaceRenderer : public Worker<TDatums>
    {
    public:
        explicit WFaceRenderer(const std::shared_ptr<FaceRenderer>& faceRenderer);

        void initializationOnThread();

        void work(TDatums& tDatums);

    private:
        std::shared_ptr<FaceRenderer> spFaceRenderer;

        DELETE_COPY(WFaceRenderer);
    };
}





// Implementation
#include <openpose/utilities/errorAndLog.hpp>
#include <openpose/core/macros.hpp>
#include <openpose/utilities/pointerContainer.hpp>
#include <openpose/utilities/profiler.hpp>
namespace op
{
    template<typename TDatums>
    WFaceRenderer<TDatums>::WFaceRenderer(const std::shared_ptr<FaceRenderer>& faceRenderer) :
        spFaceRenderer{faceRenderer}
    {
    }

    template<typename TDatums>
    void WFaceRenderer<TDatums>::initializationOnThread()
    {
        spFaceRenderer->initializationOnThread();
    }

    template<typename TDatums>
    void WFaceRenderer<TDatums>::work(TDatums& tDatums)
    {
        try
        {
            if (checkNoNullNorEmpty(tDatums))
            {
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
                // Profiling speed
                const auto profilerKey = Profiler::timerInit(__LINE__, __FUNCTION__, __FILE__);
                // Render people face
                for (auto& tDatum : *tDatums)
                    spFaceRenderer->renderFace(tDatum.outputData, tDatum.faceKeypoints);
                // Profiling speed
                Profiler::timerEnd(profilerKey);
                Profiler::printAveragedTimeMsOnIterationX(profilerKey, __LINE__, __FUNCTION__, __FILE__, Profiler::DEFAULT_X);
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            }
        }
        catch (const std::exception& e)
        {
            this->stop();
            tDatums = nullptr;
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    COMPILE_TEMPLATE_DATUM(WFaceRenderer);
}

#endif // OPENPOSE_FACE_W_FACE_RENDERER_HPP
