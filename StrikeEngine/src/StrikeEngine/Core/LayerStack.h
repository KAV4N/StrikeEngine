#pragma once

#include "StrikeEngine/Core/Core.h"
#include "Layer.h"

#include <vector>
#include <algorithm>

namespace StrikeEngine {

    template<typename T>
    class LayerStack {
    public:
        LayerStack() : mLayerInsertIndex(0) {
        }

        ~LayerStack() {
        }

        void pushLayer(T layer) {
            mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
            mLayerInsertIndex++;
        }

        void pushOverlay(T overlay) {
            mLayers.emplace_back(overlay);
        }

        void popLayer(T layer) {
            auto it = std::find(mLayers.begin(), mLayers.begin() + mLayerInsertIndex, layer);
            if (it != mLayers.begin() + mLayerInsertIndex) {
                mLayers.erase(it);
                mLayerInsertIndex--;
            }
        }

        void popOverlay(T overlay) {
            auto it = std::find(mLayers.begin() + mLayerInsertIndex, mLayers.end(), overlay);
            if (it != mLayers.end()) {
                mLayers.erase(it);
            }
        }

        void clearLayers() {
            mLayers.erase(mLayers.begin(), mLayers.begin() + mLayerInsertIndex);
            mLayerInsertIndex = 0;
        }

        void clearOverlays() {
            mLayers.erase(mLayers.begin() + mLayerInsertIndex, mLayers.end());
        }

        void clearAll() {
            mLayers.clear();
            mLayerInsertIndex = 0;
        }

        const std::vector<T>& getAllLayers() const { return mLayers; }

        typename std::vector<T>::iterator begin() { return mLayers.begin(); }
        typename std::vector<T>::iterator end() { return mLayers.end(); }

    private:
        std::vector<T> mLayers;
        uint32_t mLayerInsertIndex;
    };
}