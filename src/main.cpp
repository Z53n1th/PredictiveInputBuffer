#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <vector>
#include <cmath>

using namespace geode::prelude;

static constexpr int BLACK_CHANNEL = 1010; // GD's built-in Black color
static constexpr float DEPTH = 4.f;        // 3D offset, up and right
static constexpr float GRID = 30.f;        // one block

static bool hasObjectAt(std::vector<GameObject*> const& objs, CCPoint p) {
    for (auto o : objs) {
        auto q = o->getPosition();
        if (std::abs(q.x - p.x) < 2.f && std::abs(q.y - p.y) < 2.f) return true;
    }
    return false;
}

class $modify(AutoDecoUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        auto spr = ButtonSprite::create("Deco");
        spr->setScale(0.7f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(AutoDecoUI::onAutoDeco)
        );
        btn->setID("auto-deco-button"_spr);

        auto menu = CCMenu::create();
        menu->setID("auto-deco-menu"_spr);
        menu->setPosition({0.f, 0.f});
        menu->addChild(btn);

        auto win = CCDirector::get()->getWinSize();
        btn->setPosition({win.width * 0.32f, win.height * 0.94f});

        this->addChild(menu, 100);
        return true;
    }

    GameObject* makeBlackCopy(GameObject* src, CCPoint pos) {
        auto copy = m_editorLayer->createObject(src->m_objectID, pos, true);
        if (!copy) return nullptr;
        copy->setRotation(src->getRotation());
        copy->setScaleX(src->getScaleX());
        copy->setScaleY(src->getScaleY());
        if (copy->m_baseColor) copy->m_baseColor->m_colorID = BLACK_CHANNEL;
        if (copy->m_detailColor) copy->m_detailColor->m_colorID = BLACK_CHANNEL;
        copy->m_zOrder = src->m_zOrder - 1;
        return copy;
    }

    void onAutoDeco(CCObject*) {
        auto selected = this->getSelectedObjects();
        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("Auto Deco", "Select some blocks first.", "OK")->show();
            return;
        }

        std::vector<GameObject*> objs;
        for (auto obj : CCArrayExt<GameObject*>(selected)) objs.push_back(obj);

        int added = 0;
        for (auto obj : objs) {
            auto p = obj->getPosition();

            // Rule 1: black fill directly under the block
            if (makeBlackCopy(obj, p)) added++;

            // Rule 2: 3D depth, only on exposed sides (no selected block above and to the right)
            bool topTaken = hasObjectAt(objs, {p.x, p.y + GRID});
            bool rightTaken = hasObjectAt(objs, {p.x + GRID, p.y});
            if (topTaken && rightTaken) continue;
            if (makeBlackCopy(obj, {p.x + DEPTH, p.y + DEPTH})) added++;
        }

        FLAlertLayer::create(
            "Auto Deco", fmt::format("Added {} objects.", added), "OK"
        )->show();
    }
};
