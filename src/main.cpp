#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <set>
#include <cmath>

using namespace geode::prelude;

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
        btn->setPosition({win.width * 0.345f, win.height * 0.94f});

        this->addChild(menu, 100);
        return true;
    }

    void onAutoDeco(CCObject*) {
        auto selected = this->getSelectedObjects();
        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("Auto Deco", "Select some blocks first.", "OK")->show();
            return;
        }

        std::set<std::pair<int, int>> taken;
        for (auto obj : CCArrayExt<GameObject*>(selected)) {
            auto p = obj->getPosition();
            taken.insert({(int)std::round(p.x), (int)std::round(p.y)});
        }

        int added = 0;
        for (auto obj : CCArrayExt<GameObject*>(selected)) {
            auto p = obj->getPosition();
            int x = (int)std::round(p.x);
            int y = (int)std::round(p.y);
            if (taken.count({x, y + 30})) continue;
            m_editorLayer->createObject(40, {p.x, p.y + 30.f}, true);
            added++;
        }

        FLAlertLayer::create(
            "Auto Deco", fmt::format("Added {} objects.", added), "OK"
        )->show();
    }
};
