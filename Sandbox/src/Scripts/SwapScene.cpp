#include "SwapScene.h"

void SwapScene::onStart()
{
    auto& gd = StrikeEngine::GameData::get();

    if (!gd.hasKey("session_id"))
        initGameData();

    onSceneEnter();

    buildHUD();
    updateHUD();
    mInitialized = true;
}

void SwapScene::onUpdate(float dt)
{
    if (!mInitialized) return;

    mTimeInScene += dt;

    bool fDown = StrikeEngine::Input::isKeyPressed(STRIKE_KEY_F);
    if (fDown && !mFWasDown)
        doSwap();
    mFWasDown = fDown;

    bool tDown = StrikeEngine::Input::isKeyPressed(STRIKE_KEY_T);
    if (tDown && !mTWasDown)
    {
        auto& gd = StrikeEngine::GameData::get();
        bool current = gd.getBool("swap_sound_enabled", true);
        gd.setBool("swap_sound_enabled", !current);

        STRIKE_INFO("swap_sound_enabled toggled -> {}", !current ? "ON" : "OFF");
        updateHUD();
    }
    mTWasDown = tDown;

    if (static_cast<int>(mTimeInScene * 2.0f) !=
        static_cast<int>((mTimeInScene - dt) * 2.0f))
        updateHUD();
}

void SwapScene::doSwap()
{
    auto& gd = StrikeEngine::GameData::get();

    onSceneLeave();

    int swapCount = gd.getInt("swap_count", 0);
    int nextIndex = swapCount % SCENE_COUNT;
    std::string nextScene = SCENES[nextIndex];

    gd.setInt("swap_count", swapCount + 1);

    int score = gd.getInt("player_score", 0);
    gd.setInt("player_score", score + 100);

    gd.setString("current_scene", nextScene);
    gd.setFloat("last_swap_time", mTimeInScene);

    printGameDataSummary();

    STRIKE_INFO("Swapping to: {}", nextScene);
    StrikeEngine::World::get().loadScene(nextScene);
}

void SwapScene::initGameData()
{
    auto& gd = StrikeEngine::GameData::get();

    gd.setString("session_id", "SESSION_001");
    gd.setString("current_scene", SCENES[0]);
    gd.setInt("swap_count", 0);
    gd.setInt("player_score", 0);
    gd.setFloat("last_swap_time", 0.0f);
    gd.setDouble("total_time", 0.0);
    gd.setBool("swap_sound_enabled", true);

    STRIKE_INFO("GameData initialised for new session.");
    STRIKE_INFO("getDataCount() = {}", gd.getDataCount());
}

void SwapScene::onSceneEnter()
{
    auto& gd = StrikeEngine::GameData::get();

    int    swapCount = gd.getInt("swap_count", 0);
    int    score     = gd.getInt("player_score", 0);
    float  lastSwap  = gd.getFloat("last_swap_time", 0.0f);
    double totalTime = gd.getDouble("total_time", 0.0);
    bool   soundOn   = gd.getBool("swap_sound_enabled", true);
    std::string sid  = gd.getString("session_id", "?");
    std::string scene = gd.getString("current_scene", "?");

    STRIKE_INFO("=== Scene entered ===");
    STRIKE_INFO("session_id        = {}", sid);
    STRIKE_INFO("current_scene     = {}", scene);
    STRIKE_INFO("swap_count        = {}", swapCount);
    STRIKE_INFO("player_score      = {}", score);
    STRIKE_INFO("last_swap_time    = {:.2f}s", lastSwap);
    STRIKE_INFO("total_time        = {:.2f}s", totalTime);
    STRIKE_INFO("swap_sound_enabled= {}", soundOn ? "ON" : "OFF");

    STRIKE_INFO("All keys ({} total):", gd.getDataCount());
    for (const auto& key : gd.getAllKeys())
        STRIKE_INFO("  '{}'", key);

    mTimeInScene = 0.0f;
}

void SwapScene::onSceneLeave()
{
    auto& gd = StrikeEngine::GameData::get();

    double prev = gd.getDouble("total_time", 0.0);
    gd.setDouble("total_time", prev + static_cast<double>(mTimeInScene));

    if (gd.hasKey("temp_swap_flag"))
    {
        gd.deleteKey("temp_swap_flag");
        STRIKE_INFO("Deleted stale 'temp_swap_flag'.");
    }

    gd.setBool("temp_swap_flag", true);

    STRIKE_INFO("onSceneLeave – total_time accumulated to {}s",
        gd.getDouble("total_time", 0.0));
}

void SwapScene::printGameDataSummary()
{
    auto& gd = StrikeEngine::GameData::get();

    STRIKE_INFO("─── GameData snapshot (pre-swap) ───────────────────────────");
    gd.printAllData();
    STRIKE_INFO("getDataCount() = {}", gd.getDataCount());
    STRIKE_INFO("────────────────────────────────────────────────────────────");
}

void SwapScene::buildHUD()
{
    auto* scene = getEntity().getScene();

    auto makeText = [&](float y, const glm::uvec3& col) -> StrikeEngine::Entity
    {
        auto ent = scene->createEntity();
        auto& t  = ent.addComponent<StrikeEngine::TextComponent>();
        t.setColor(col);
        t.setPosition(glm::vec2(0.03f, y));
        t.setPivot(glm::vec2(0.0f, 0.0f));
        return ent;
    };

    mHUDLine1 = makeText(0.03f, glm::uvec3(80, 220, 255));
    mHUDLine2 = makeText(0.07f, glm::uvec3(255, 220, 80));
    mHUDLine3 = makeText(0.11f, glm::uvec3(180, 255, 180));
    mHUDLine4 = makeText(0.15f, glm::uvec3(200, 200, 200));
}

void SwapScene::updateHUD()
{
    auto& gd = StrikeEngine::GameData::get();

    std::string scene = gd.getString("current_scene", "?");
    int swaps         = gd.getInt("swap_count", 0);
    int score         = gd.getInt("player_score", 0);
    bool soundOn      = gd.getBool("swap_sound_enabled", true);
    size_t keyCount   = gd.getDataCount();

    auto slash = scene.rfind('/');
    std::string sceneName =
        (slash != std::string::npos) ? scene.substr(slash + 1) : scene;

    auto setLine = [](StrikeEngine::Entity& e, const std::string& text)
    {
        if (e.isValid())
            e.getComponent<StrikeEngine::TextComponent>().setText(text);
    };

    {
        std::ostringstream ss;
        ss << "Scene: " << sceneName
           << "  |  Swaps: " << swaps
           << "  |  Time: " << std::fixed << std::setprecision(1)
           << mTimeInScene << "s";
        setLine(mHUDLine1, ss.str());
    }

    {
        std::ostringstream ss;
        ss << "Score: " << score
           << "  |  Sound: " << (soundOn ? "ON" : "OFF")
           << "  (T = toggle)";
        setLine(mHUDLine2, ss.str());
    }

    setLine(mHUDLine3, "F = swap scene   (GameData persists across loads)");

    {
        std::ostringstream ss;
        ss << "GameData keys stored: " << keyCount;
        setLine(mHUDLine4, ss.str());
    }
}

REGISTER_SCRIPT(SwapScene);