#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#include <QQuickWindow>
#include <QQuickItem>
#include <QQuickRenderControl>

#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QImage>
#include <gameController.h>

class UiBackend;

class DxHookOverlay : public QObject
{
    Q_OBJECT

public:
    explicit DxHookOverlay(GameController* gameController, UiBackend* uiBackend, QQmlApplicationEngine* engine, QObject* parent = nullptr);
    ~DxHookOverlay();

    bool initialize(int width, int height);
    bool render();

    const QImage& image() const
    {
        return m_image;
    }

public slots:
    void runOverlay(bool gameLaunched);

private:
    bool initializeOpenGL();
    bool initializeQml();
    bool initializeFramebuffer();
    void registerQmlContext();

    bool InjectDLL(DWORD processId, const std::wstring& dllPath);
    bool UninjectDLL(DWORD processId, const std::wstring& dllPath);
    DWORD GetProcessIdByName(const std::wstring& processName);
    //bool RequestDLLUnload(DWORD processId);
    void cleanupOverlay();

private:
    UiBackend* m_uiBackend = nullptr;

    QQmlEngine* m_engine;
    QQuickRenderControl m_renderControl;

    QQuickWindow* m_window = nullptr;
    QQuickItem* m_root = nullptr;

    QOpenGLContext* m_context = nullptr;
    QOffscreenSurface* m_surface = nullptr;

    QOpenGLFramebufferObject* m_fbo = nullptr;

    QImage m_image;

    int m_width = 1920;
    int m_height = 1080;

    QTimer* m_renderTimer = nullptr;
    GameController* p_gameController;
};
