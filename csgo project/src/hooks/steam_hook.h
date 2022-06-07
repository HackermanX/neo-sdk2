#pragma once
class steam_hook
{
public:
    steam_hook() = default;
    explicit steam_hook(void* pFunction, void* pDetour)
        : pBaseFn(pFunction), pReplaceFn(pDetour) { }
    ~steam_hook() {
        this->Remove();
    }

    bool Install(void* pFunction, void* pDetour) {
        pBaseFn = pFunction;
        if (pBaseFn == nullptr)
            return false;
        pReplaceFn = pDetour;
        if (pReplaceFn == nullptr)
            return false;

        return hook(pFunction, pDetour, &pOriginalFn, 0);
    }

    bool Install2(void* pFunction, void* pDetour, void* pOriginalFn) {
        pBaseFn = pFunction;
        if (pBaseFn == nullptr)
            return false;
        pReplaceFn = pDetour;
        if (pReplaceFn == nullptr)
            return false;

        return hook(pFunction, pDetour, &pOriginalFn, 0);
    }

    void Remove() {
        unhook(pBaseFn, false);
    }

    template <typename Fn>
    Fn get_original() {
        return (Fn)pOriginalFn;
    }

    typedef bool(__cdecl* game_overlay_renderer_hook_t)(void*, void*, void*, int);
    typedef void(__cdecl* game_overlay_renderer_unhook_t)(void*, bool);
    static game_overlay_renderer_hook_t hook;
    static game_overlay_renderer_unhook_t unhook;
private:
    void* pBaseFn = nullptr;
    void* pReplaceFn = nullptr;
    void* pOriginalFn = nullptr;
};

inline steam_hook method;