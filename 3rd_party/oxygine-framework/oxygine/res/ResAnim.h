#pragma once
#include "3rd_party/oxygine-framework/oxygine/oxygine-forwards.h"
#include "3rd_party/oxygine-framework/oxygine/AnimationFrame.h"
#include "3rd_party/oxygine-framework/oxygine/core/Texture.h"
#include "3rd_party/oxygine-framework/oxygine/res/Resource.h"

namespace oxygine
{
    using animationFrames = QVector<AnimationFrame>;
    class ResAnim;
    using spResAnim = intrusive_ptr<ResAnim>;
    class ResAnim: public Resource
    {
    public:
        explicit ResAnim(Resource* atlas = nullptr);
        virtual ~ResAnim() = default;

        void init(QString file, qint32 columns, qint32 rows, float scaleFactor, bool addTransparentBorder);
        void init(QImage & img, qint32 columns, qint32 rows, float scaleFactor, bool addTransparentBorder);
        virtual void init(Image* original, qint32 columns, qint32 rows, float scaleFactor, bool addTransparentBorder);
        void init(animationFrames& frames, qint32 columns, float scaleFactor, float appliedScale = 1);
        /**creates animation frames from Texture*/
        void init(spTexture texture, const Point& originalSize, qint32 columns, qint32 rows, float scaleFactor, bool addTransparentBorder);

        /*adds additional column. use it only if rows = 1*/
        //void addFrame(const AnimationFrame &frame);

        float getScaleFactor() const
        {
            return m_scaleFactor;
        }
        float getAppliedScale() const
        {
            return m_appliedScale;
        }
        qint32 getColumns() const
        {
            return m_columns;
        }
        qint32 getRows() const
        {
            return m_frames.size() / m_columns;
        }
        qint32 getTotalFrames() const
        {
            return m_frames.size();
        }
        qint32 getFrameRate() const
        {
            return m_framerate;
        }
        const Resources* getResources() const;
        const AnimationFrame& getFrame(qint32 col, qint32 row) const;
        /**returns frame by index ignoring cols and rows*/
        const AnimationFrame& getFrame(qint32 index) const;
        /**Returns atlas where this ResAnim was created*/
        Resource* getAtlas()
        {
            return m_atlas;
        }
        /**Returns size of frame*/
        const Vector2& getSize() const;
        float getWidth() const;
        float getHeight() const;
        void setFrame(qint32 col, qint32 row, const AnimationFrame& frame);
        void setFrameRate(qint32 v)
        {
            m_framerate = v;
        }
        void removeFrames();
        operator const AnimationFrame& ();

        QString getResPath() const;
        void setResPath(const QString &value);

    protected:
        virtual void _load(LoadResourcesContext* ctx = 0) override;
        virtual void _unload() override;

    protected:
        qint32      m_columns;
        Resource*   m_atlas;
        float       m_scaleFactor;
        float       m_appliedScale;
        qint32      m_framerate;
        animationFrames m_frames;
        QString m_resPath;

    private:
        static AnimationFrame m_emptyFrame;
    };
}
