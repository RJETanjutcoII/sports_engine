// main.cpp
// Application entry point and game loop for Sports Engine.
#include "Core/Logger.hpp"
#include "Core/Timer.hpp"
#include "Core/Types.hpp"
#include "Renderer/Window.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Primitives.hpp"
#include "Game/Ball.hpp"
#include "Game/Player.hpp"
#include "Game/AIPlayer.hpp"
#include "Game/Match.hpp"
#include "Input/InputHandler.hpp"

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace Sports;

class Application {
public:
    bool init();
    void run();
    void shutdown();

private:
    void processInput(f32 deltaTime);
    void update(f32 deltaTime);
    void render();
    void createScene();
    void drawGoalCelebration();

    Window m_window;
    Camera m_camera;
    Shader m_shader;
    Timer m_frameTimer;

    // Scene meshes
    Mesh m_fieldMesh;
    Mesh m_ballMesh;
    Mesh m_playerMesh;
    Mesh m_playerFaceMesh;
    std::vector<Mesh> m_fieldLines;
    Mesh m_goalPostMesh;
    Mesh m_crossbarMesh;
    Mesh m_aiPlayerMeshRed;
    Mesh m_aiPlayerFaceMeshRed;
    Mesh m_aiPlayerMeshBlue;
    Mesh m_aiPlayerFaceMeshBlue;

    // Field dimensions (FIFA standard in meters)
    static constexpr f32 FIELD_LENGTH = 105.0f;
    static constexpr f32 FIELD_WIDTH = 68.0f;
    static constexpr f32 PENALTY_AREA_LENGTH = 16.5f;
    static constexpr f32 PENALTY_AREA_WIDTH = 40.32f;
    static constexpr f32 GOAL_AREA_LENGTH = 5.5f;
    static constexpr f32 GOAL_AREA_WIDTH = 18.32f;
    static constexpr f32 CENTER_CIRCLE_RADIUS = 9.15f;
    static constexpr f32 GOAL_WIDTH = 7.32f;
    static constexpr f32 GOAL_HEIGHT = 2.44f;
    static constexpr f32 LINE_WIDTH = 0.12f;

    // Game objects
    Ball m_ball;
    Player m_player;
    AIManager m_aiManager;
    Match m_match;
    InputHandler m_input;

    FieldBounds m_fieldBounds;

    bool m_aiEnabled = true;

    // Simple directional lighting
    Vec3 m_lightDir = glm::normalize(Vec3(0.5f, 1.0f, 0.3f));
    Vec3 m_lightColor{1.0f, 1.0f, 0.95f};
    Vec3 m_ambientColor{0.3f, 0.3f, 0.35f};
};

bool Application::init() {
    Logger::init();
    LOG_INFO("Starting Sports Engine...");

    WindowConfig windowConfig;
    windowConfig.title = "Sports Engine - Third Person Camera";
    windowConfig.width = 1600;
    windowConfig.height = 900;

    if (!m_window.init(windowConfig)) {
        LOG_ERROR("Failed to initialize window");
        return false;
    }

    // Lock mouse to window for camera control
    m_input.setMouseCaptured(true);

    // Configure third-person camera
    m_camera.setPerspective(60.0f, m_window.getAspectRatio(), 0.1f, 500.0f);
    m_camera.setFollowDistance(8.0f);
    m_camera.setFollowHeight(3.0f);
    m_camera.setSensitivity(0.003f);

    if (!m_shader.loadFromFiles("assets/shaders/basic.vert", "assets/shaders/basic.frag")) {
        LOG_ERROR("Failed to load shaders");
        return false;
    }

    // Initialize field bounds for physics
    m_fieldBounds.length = FIELD_LENGTH;
    m_fieldBounds.width = FIELD_WIDTH;
    m_fieldBounds.goalWidth = GOAL_WIDTH;
    m_fieldBounds.goalHeight = GOAL_HEIGHT;

    m_match.setFieldDimensions(FIELD_LENGTH, FIELD_WIDTH, GOAL_WIDTH, GOAL_HEIGHT);
    m_aiManager.createTeams(FIELD_LENGTH);

    createScene();

    LOG_INFO("Application initialized successfully");
    LOG_INFO("Controls:");
    LOG_INFO("  WASD - Move player");
    LOG_INFO("  Shift - Sprint");
    LOG_INFO("  Space - Kick ball");
    LOG_INFO("  Left Click + Space - Kick with left spin (curves right)");
    LOG_INFO("  Right Click + Space - Kick with right spin (curves left)");
    LOG_INFO("  Mouse - Look around");
    LOG_INFO("  Scroll - Zoom in/out");
    LOG_INFO("  Tab - Toggle mouse capture");
    LOG_INFO("  R - Reset ball");
    LOG_INFO("  0 - Toggle AI (for testing)");
    LOG_INFO("  Escape - Quit");

    return true;
}

void Application::createScene() {
    // Generate all meshes using procedural primitives

    Vec3 grassColor(0.2f, 0.5f, 0.2f);
    auto [fieldVerts, fieldIndices] = Primitives::createPlane(FIELD_LENGTH, FIELD_WIDTH, grassColor, 4);
    m_fieldMesh.upload(fieldVerts, fieldIndices);

    Vec3 ballColor(1.0f, 1.0f, 1.0f);
    auto [ballVerts, ballIndices] = Primitives::createSphere(Ball::RADIUS, ballColor, 16, 32);
    m_ballMesh.upload(ballVerts, ballIndices);

    // Human player (blue team)
    Vec3 playerColor(0.2f, 0.4f, 0.8f);
    auto [playerVerts, playerIndices] = Primitives::createCapsule(Player::RADIUS, 1.8f, playerColor, 8, 16);
    m_playerMesh.upload(playerVerts, playerIndices);

    // Direction indicator cone
    Vec3 faceColor(1.0f, 0.9f, 0.2f);
    auto [faceVerts, faceIndices] = Primitives::createCone(0.15f, 0.4f, faceColor, 12);
    m_playerFaceMesh.upload(faceVerts, faceIndices);

    // Field line helper
    Vec3 lineColor(1.0f, 1.0f, 1.0f);
    f32 lineY = 0.02f;  // Slightly above ground to prevent z-fighting
    f32 halfLength = FIELD_LENGTH / 2.0f;
    f32 halfWidth = FIELD_WIDTH / 2.0f;

    auto addLine = [&](const Vec3& start, const Vec3& end) {
        auto [verts, indices] = Primitives::createLine(start, end, LINE_WIDTH, lineColor);
        m_fieldLines.emplace_back();
        m_fieldLines.back().upload(verts, indices);
    };

    // Boundary lines
    addLine(Vec3(-halfLength, lineY, -halfWidth), Vec3(halfLength, lineY, -halfWidth));
    addLine(Vec3(-halfLength, lineY, halfWidth), Vec3(halfLength, lineY, halfWidth));
    addLine(Vec3(-halfLength, lineY, -halfWidth), Vec3(-halfLength, lineY, halfWidth));
    addLine(Vec3(halfLength, lineY, -halfWidth), Vec3(halfLength, lineY, halfWidth));

    // Center line
    addLine(Vec3(0.0f, lineY, -halfWidth), Vec3(0.0f, lineY, halfWidth));

    // Center circle (approximated with line segments)
    const int circleSegments = 32;
    for (int i = 0; i < circleSegments; i++) {
        f32 angle1 = (f32)i / circleSegments * 6.28318f;
        f32 angle2 = (f32)(i + 1) / circleSegments * 6.28318f;
        Vec3 p1(CENTER_CIRCLE_RADIUS * std::cos(angle1), lineY, CENTER_CIRCLE_RADIUS * std::sin(angle1));
        Vec3 p2(CENTER_CIRCLE_RADIUS * std::cos(angle2), lineY, CENTER_CIRCLE_RADIUS * std::sin(angle2));
        addLine(p1, p2);
    }

    // Penalty areas
    f32 penaltyHalfWidth = PENALTY_AREA_WIDTH / 2.0f;
    addLine(Vec3(-halfLength, lineY, -penaltyHalfWidth), Vec3(-halfLength + PENALTY_AREA_LENGTH, lineY, -penaltyHalfWidth));
    addLine(Vec3(-halfLength, lineY, penaltyHalfWidth), Vec3(-halfLength + PENALTY_AREA_LENGTH, lineY, penaltyHalfWidth));
    addLine(Vec3(-halfLength + PENALTY_AREA_LENGTH, lineY, -penaltyHalfWidth), Vec3(-halfLength + PENALTY_AREA_LENGTH, lineY, penaltyHalfWidth));
    addLine(Vec3(halfLength, lineY, -penaltyHalfWidth), Vec3(halfLength - PENALTY_AREA_LENGTH, lineY, -penaltyHalfWidth));
    addLine(Vec3(halfLength, lineY, penaltyHalfWidth), Vec3(halfLength - PENALTY_AREA_LENGTH, lineY, penaltyHalfWidth));
    addLine(Vec3(halfLength - PENALTY_AREA_LENGTH, lineY, -penaltyHalfWidth), Vec3(halfLength - PENALTY_AREA_LENGTH, lineY, penaltyHalfWidth));

    // Goal areas
    f32 goalAreaHalfWidth = GOAL_AREA_WIDTH / 2.0f;
    addLine(Vec3(-halfLength, lineY, -goalAreaHalfWidth), Vec3(-halfLength + GOAL_AREA_LENGTH, lineY, -goalAreaHalfWidth));
    addLine(Vec3(-halfLength, lineY, goalAreaHalfWidth), Vec3(-halfLength + GOAL_AREA_LENGTH, lineY, goalAreaHalfWidth));
    addLine(Vec3(-halfLength + GOAL_AREA_LENGTH, lineY, -goalAreaHalfWidth), Vec3(-halfLength + GOAL_AREA_LENGTH, lineY, goalAreaHalfWidth));
    addLine(Vec3(halfLength, lineY, -goalAreaHalfWidth), Vec3(halfLength - GOAL_AREA_LENGTH, lineY, -goalAreaHalfWidth));
    addLine(Vec3(halfLength, lineY, goalAreaHalfWidth), Vec3(halfLength - GOAL_AREA_LENGTH, lineY, goalAreaHalfWidth));
    addLine(Vec3(halfLength - GOAL_AREA_LENGTH, lineY, -goalAreaHalfWidth), Vec3(halfLength - GOAL_AREA_LENGTH, lineY, goalAreaHalfWidth));

    // Goal posts and crossbars
    Vec3 goalColor(1.0f, 1.0f, 1.0f);
    f32 postRadius = 0.06f;

    auto [postVerts, postIndices] = Primitives::createCapsule(postRadius, GOAL_HEIGHT, goalColor, 8, 16);
    m_goalPostMesh.upload(postVerts, postIndices);

    auto [barVerts, barIndices] = Primitives::createCapsule(postRadius, GOAL_WIDTH, goalColor, 8, 16);
    m_crossbarMesh.upload(barVerts, barIndices);

    // AI players - Red team
    Vec3 redColor(0.8f, 0.2f, 0.2f);
    auto [aiRedVerts, aiRedIndices] = Primitives::createCapsule(AIPlayer::RADIUS, 1.8f, redColor, 8, 16);
    m_aiPlayerMeshRed.upload(aiRedVerts, aiRedIndices);

    Vec3 redFaceColor(1.0f, 0.5f, 0.2f);
    auto [redFaceVerts, redFaceIndices] = Primitives::createCone(0.15f, 0.4f, redFaceColor, 12);
    m_aiPlayerFaceMeshRed.upload(redFaceVerts, redFaceIndices);

    // AI players - Blue team
    Vec3 blueColor(0.2f, 0.4f, 0.8f);
    auto [aiBlueVerts, aiBlueIndices] = Primitives::createCapsule(AIPlayer::RADIUS, 1.8f, blueColor, 8, 16);
    m_aiPlayerMeshBlue.upload(aiBlueVerts, aiBlueIndices);

    Vec3 blueFaceColor(0.3f, 0.7f, 1.0f);
    auto [blueFaceVerts, blueFaceIndices] = Primitives::createCone(0.15f, 0.4f, blueFaceColor, 12);
    m_aiPlayerFaceMeshBlue.upload(blueFaceVerts, blueFaceIndices);

    LOG_INFO("Scene created with field markings, goals, and {} AI players", m_aiManager.getPlayers().size());
}

void Application::run() {
    LOG_INFO("Entering main loop");

    while (!m_window.shouldClose()) {
        // Calculate frame delta time (capped to prevent physics explosions)
        f32 deltaTime = static_cast<f32>(m_frameTimer.lap());
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        processInput(deltaTime);
        update(deltaTime);
        render();

        m_window.swapBuffers();
    }
}

void Application::processInput(f32 deltaTime) {
    m_input.processEvents(m_window, m_camera);
    m_input.updateKeyboardState(m_camera);

    // Handle debug/reset controls
    if (m_input.shouldResetBall()) {
        m_ball.reset();
        m_input.clearResetBall();
    }

    if (m_input.shouldToggleAI()) {
        m_aiEnabled = !m_aiEnabled;
        LOG_INFO("AI: {}", m_aiEnabled ? "ENABLED" : "DISABLED");
        m_input.clearToggleAI();
    }

    // Pass input to player controller
    const auto& inputState = m_input.getState();
    m_player.setMovementInput(inputState.movementDirection, inputState.sprinting);
    m_player.setTargetRotation(-m_camera.getYaw());  // Face camera direction

    // Kick attempt
    if (inputState.kickJustPressed && !m_match.isGoalScored()) {
        m_player.tryKick(m_ball, inputState.sprinting, inputState.spinY);
    }
}

void Application::update(f32 deltaTime) {
    // Player movement bounds
    Vec3 boundsMin(-FIELD_LENGTH / 2.0f + 1.0f, 0.0f, -FIELD_WIDTH / 2.0f + 1.0f);
    Vec3 boundsMax(FIELD_LENGTH / 2.0f - 1.0f, 0.0f, FIELD_WIDTH / 2.0f - 1.0f);

    m_player.update(deltaTime, boundsMin, boundsMax);

    // Camera follows player
    m_camera.setFollowTarget(m_player.getPosition());
    m_camera.setAspectRatio(m_window.getAspectRatio());
    m_camera.update(deltaTime);

    // Ball physics
    m_ball.update(deltaTime, m_fieldBounds);
    m_match.handleBoundaryCollision(m_ball);

    // Player-ball interaction
    if (!m_match.isGoalScored()) {
        m_player.handleBallCollision(m_ball, deltaTime);
    }

    // Goal detection and celebration
    m_match.update(deltaTime, m_ball);

    // AI team updates
    if (m_aiEnabled) {
        m_aiManager.update(deltaTime, m_ball, m_player.getPosition(),
                          FIELD_LENGTH, FIELD_WIDTH, GOAL_WIDTH);
    }
}

void Application::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader.bind();

    // Upload lighting uniforms
    m_shader.setVec3("uLightDir", m_lightDir);
    m_shader.setVec3("uLightColor", m_lightColor);
    m_shader.setVec3("uAmbientColor", m_ambientColor);
    m_shader.setVec3("uCameraPos", m_camera.getPosition());

    // Upload camera matrices
    m_shader.setMat4("uView", m_camera.getViewMatrix());
    m_shader.setMat4("uProjection", m_camera.getProjectionMatrix());

    // Draw field
    Mat4 fieldModel = Mat4(1.0f);
    m_shader.setMat4("uModel", fieldModel);
    m_fieldMesh.draw();

    // Draw field markings
    for (auto& line : m_fieldLines) {
        m_shader.setMat4("uModel", Mat4(1.0f));
        line.draw();
    }

    // Draw goals (4 posts + 2 crossbars)
    f32 halfLength = FIELD_LENGTH / 2.0f;
    f32 goalHalfWidth = GOAL_WIDTH / 2.0f;

    // Left goal posts
    Mat4 postModel = glm::translate(Mat4(1.0f), Vec3(-halfLength, GOAL_HEIGHT / 2.0f, -goalHalfWidth));
    m_shader.setMat4("uModel", postModel);
    m_goalPostMesh.draw();

    postModel = glm::translate(Mat4(1.0f), Vec3(-halfLength, GOAL_HEIGHT / 2.0f, goalHalfWidth));
    m_shader.setMat4("uModel", postModel);
    m_goalPostMesh.draw();

    // Left crossbar (rotated 90 degrees)
    Mat4 barModel = glm::translate(Mat4(1.0f), Vec3(-halfLength, GOAL_HEIGHT, 0.0f));
    barModel = glm::rotate(barModel, glm::radians(90.0f), Vec3(1.0f, 0.0f, 0.0f));
    m_shader.setMat4("uModel", barModel);
    m_crossbarMesh.draw();

    // Right goal posts
    postModel = glm::translate(Mat4(1.0f), Vec3(halfLength, GOAL_HEIGHT / 2.0f, -goalHalfWidth));
    m_shader.setMat4("uModel", postModel);
    m_goalPostMesh.draw();

    postModel = glm::translate(Mat4(1.0f), Vec3(halfLength, GOAL_HEIGHT / 2.0f, goalHalfWidth));
    m_shader.setMat4("uModel", postModel);
    m_goalPostMesh.draw();

    // Right crossbar
    barModel = glm::translate(Mat4(1.0f), Vec3(halfLength, GOAL_HEIGHT, 0.0f));
    barModel = glm::rotate(barModel, glm::radians(90.0f), Vec3(1.0f, 0.0f, 0.0f));
    m_shader.setMat4("uModel", barModel);
    m_crossbarMesh.draw();

    // Draw ball with rotation
    Mat4 ballModel = glm::translate(Mat4(1.0f), m_ball.getPosition());
    ballModel = glm::rotate(ballModel, m_ball.getRotationAngle(), Vec3(1.0f, 0.0f, 0.0f));
    m_shader.setMat4("uModel", ballModel);
    m_ballMesh.draw();

    // Draw human player with animation
    f32 playerSpeed = m_player.getSpeed();
    f32 bobAmount = 0.0f;
    f32 leanAngle = 0.0f;

    if (playerSpeed > 0.5f) {
        // Running bob animation
        bobAmount = std::sin(m_player.getAnimationTime() * 2.0f) * 0.05f * std::min(playerSpeed / 8.0f, 1.0f);
        leanAngle = std::min(playerSpeed / 15.0f, 0.15f);
    }

    if (m_player.isKicking()) {
        // Kick lean animation
        f32 kickProgress = m_player.getKickTimer() / 0.3f;
        f32 kickLean = std::sin(kickProgress * 3.14159f) * 0.3f;
        leanAngle += kickLean;
    }

    Vec3 playerRenderPos = m_player.getPosition() + Vec3(0.0f, 0.9f + bobAmount, 0.0f);
    Mat4 playerModel = glm::translate(Mat4(1.0f), playerRenderPos);
    playerModel = glm::rotate(playerModel, m_player.getRotation(), Vec3(0.0f, 1.0f, 0.0f));
    playerModel = glm::rotate(playerModel, leanAngle, Vec3(1.0f, 0.0f, 0.0f));
    m_shader.setMat4("uModel", playerModel);
    m_playerMesh.draw();

    // Draw player face indicator (shows direction)
    f32 faceOffsetDist = 0.35f;
    Vec3 faceForward(-std::sin(m_player.getRotation()), 0.0f, -std::cos(m_player.getRotation()));
    Vec3 faceRenderPos = m_player.getPosition() + Vec3(0.0f, 1.4f + bobAmount, 0.0f) + faceForward * faceOffsetDist;

    Mat4 faceModel = glm::translate(Mat4(1.0f), faceRenderPos);
    faceModel = glm::rotate(faceModel, m_player.getRotation(), Vec3(0.0f, 1.0f, 0.0f));
    faceModel = glm::rotate(faceModel, glm::radians(90.0f), Vec3(1.0f, 0.0f, 0.0f));
    m_shader.setMat4("uModel", faceModel);
    m_playerFaceMesh.draw();

    // Draw AI players
    for (const auto& ai : m_aiManager.getPlayers()) {
        f32 aiSpeed = glm::length(ai.getVelocity());

        f32 aiBob = 0.0f;
        f32 aiLean = 0.0f;
        if (aiSpeed > 0.5f) {
            aiBob = std::sin(ai.getAnimTime() * 2.0f) * 0.05f * std::min(aiSpeed / 7.0f, 1.0f);
            aiLean = std::min(aiSpeed / 12.0f, 0.15f);
        }

        // Select mesh based on team
        Mesh& bodyMesh = (ai.getTeam() == 0) ? m_aiPlayerMeshRed : m_aiPlayerMeshBlue;
        Mesh& faceMesh = (ai.getTeam() == 0) ? m_aiPlayerFaceMeshRed : m_aiPlayerFaceMeshBlue;

        Vec3 aiRenderPos = ai.getPosition() + Vec3(0.0f, 0.9f + aiBob, 0.0f);
        Mat4 aiModel = glm::translate(Mat4(1.0f), aiRenderPos);
        aiModel = glm::rotate(aiModel, ai.getRotation(), Vec3(0.0f, 1.0f, 0.0f));
        aiModel = glm::rotate(aiModel, aiLean, Vec3(1.0f, 0.0f, 0.0f));
        m_shader.setMat4("uModel", aiModel);
        bodyMesh.draw();

        // AI face indicator
        f32 aiFaceOffset = 0.35f;
        Vec3 aiForward(-std::sin(ai.getRotation()), 0.0f, -std::cos(ai.getRotation()));
        Vec3 aiFacePos = ai.getPosition() + Vec3(0.0f, 1.4f + aiBob, 0.0f) + aiForward * aiFaceOffset;

        Mat4 aiFaceModel = glm::translate(Mat4(1.0f), aiFacePos);
        aiFaceModel = glm::rotate(aiFaceModel, ai.getRotation(), Vec3(0.0f, 1.0f, 0.0f));
        aiFaceModel = glm::rotate(aiFaceModel, glm::radians(90.0f), Vec3(1.0f, 0.0f, 0.0f));
        m_shader.setMat4("uModel", aiFaceModel);
        faceMesh.draw();
    }

    // Draw goal celebration overlay
    if (m_match.isGoalScored()) {
        drawGoalCelebration();
    }

    m_shader.unbind();
}

void Application::drawGoalCelebration() {
    f32 alpha = m_match.getCelebrationAlpha();
    if (alpha <= 0.0f) return;

    // Team-colored text
    Vec3 textColor = (m_match.getLastScoringTeam() == 0)
        ? Vec3(1.0f, 0.3f, 0.3f)   // Red team scored
        : Vec3(0.3f, 0.5f, 1.0f);  // Blue team scored

    textColor *= alpha;  // Fade effect

    i32 winWidth, winHeight;
    SDL_GetWindowSize(m_window.getSDLWindow(), &winWidth, &winHeight);

    // Switch to orthographic projection for 2D overlay
    Mat4 orthoProj = glm::ortho(0.0f, static_cast<f32>(winWidth),
                                 0.0f, static_cast<f32>(winHeight),
                                 -1.0f, 1.0f);
    Mat4 identityView = Mat4(1.0f);

    m_shader.setMat4("uProjection", orthoProj);
    m_shader.setMat4("uView", identityView);

    glDisable(GL_DEPTH_TEST);

    // Pixel art style "GOAL!" text using balls as blocks
    f32 blockSize = 20.0f;
    f32 spacing = 25.0f;
    f32 centerX = winWidth / 2.0f;
    f32 centerY = winHeight / 2.0f + 50.0f;

    auto drawBlock = [&](f32 gridX, f32 gridY) {
        f32 screenX = centerX + gridX * spacing;
        f32 screenY = centerY + gridY * spacing;

        Mat4 model = glm::translate(Mat4(1.0f), Vec3(screenX, screenY, 0.0f));
        model = glm::scale(model, Vec3(blockSize, blockSize, 1.0f));

        m_shader.setVec3("uAmbientColor", textColor);
        m_shader.setVec3("uLightColor", Vec3(0.0f));  // No lighting for 2D
        m_shader.setMat4("uModel", model);
        m_ballMesh.draw();
    };

    // "GOAL!" spelled out in blocks (5x5 grid per character)
    // G
    drawBlock(-8.0f, 2.0f); drawBlock(-8.0f, 1.0f); drawBlock(-8.0f, 0.0f);
    drawBlock(-8.0f, -1.0f); drawBlock(-8.0f, -2.0f);
    drawBlock(-7.0f, 2.0f); drawBlock(-6.0f, 2.0f);
    drawBlock(-7.0f, -2.0f); drawBlock(-6.0f, -2.0f);
    drawBlock(-6.0f, -1.0f); drawBlock(-6.0f, 0.0f); drawBlock(-7.0f, 0.0f);

    // O
    drawBlock(-4.0f, 2.0f); drawBlock(-4.0f, 1.0f); drawBlock(-4.0f, 0.0f);
    drawBlock(-4.0f, -1.0f); drawBlock(-4.0f, -2.0f);
    drawBlock(-3.0f, 2.0f); drawBlock(-2.0f, 2.0f);
    drawBlock(-3.0f, -2.0f); drawBlock(-2.0f, -2.0f);
    drawBlock(-2.0f, 1.0f); drawBlock(-2.0f, 0.0f); drawBlock(-2.0f, -1.0f);

    // A
    drawBlock(0.0f, 1.0f); drawBlock(0.0f, 0.0f);
    drawBlock(0.0f, -1.0f); drawBlock(0.0f, -2.0f);
    drawBlock(1.0f, 2.0f);
    drawBlock(1.0f, 0.0f);
    drawBlock(2.0f, 1.0f); drawBlock(2.0f, 0.0f);
    drawBlock(2.0f, -1.0f); drawBlock(2.0f, -2.0f);

    // L
    drawBlock(4.0f, 2.0f); drawBlock(4.0f, 1.0f); drawBlock(4.0f, 0.0f);
    drawBlock(4.0f, -1.0f); drawBlock(4.0f, -2.0f);
    drawBlock(5.0f, -2.0f); drawBlock(6.0f, -2.0f);

    // !
    drawBlock(8.0f, 2.0f); drawBlock(8.0f, 1.0f); drawBlock(8.0f, 0.0f);
    drawBlock(8.0f, -2.0f);

    // Restore 3D rendering state
    glEnable(GL_DEPTH_TEST);
    m_shader.setMat4("uProjection", m_camera.getProjectionMatrix());
    m_shader.setMat4("uView", m_camera.getViewMatrix());
    m_shader.setVec3("uAmbientColor", m_ambientColor);
    m_shader.setVec3("uLightColor", m_lightColor);
}

void Application::shutdown() {
    LOG_INFO("Shutting down...");
    m_input.setMouseCaptured(false);
    m_window.shutdown();
    Logger::shutdown();
}

int main(int argc, char* argv[]) {
    Application app;

    if (!app.init()) {
        return 1;
    }

    app.run();
    app.shutdown();

    return 0;
}
