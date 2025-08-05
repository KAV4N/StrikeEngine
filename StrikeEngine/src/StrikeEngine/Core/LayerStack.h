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

        void pushLayer(T layer, bool active = true) {
            mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
            mLayerInsertIndex++;
            if (active) {
                mActiveLayers.push_back(layer);
            }
        }

        void pushOverlay(T overlay, bool active = true) {
            mLayers.emplace_back(overlay);
            if (active) {
                mActiveLayers.push_back(overlay);
            }
        }

        void popLayer(T layer) {
            auto it = std::find(mLayers.begin(), mLayers.end(), layer);
            if (it != mLayers.end()) {
                mLayers.erase(it);
                mLayerInsertIndex--;
            }

            auto activeIt = std::find(mActiveLayers.begin(), mActiveLayers.end(), layer);
            if (activeIt != mActiveLayers.end()) {
                mActiveLayers.erase(activeIt);
            }
        }

        void popOverlay(T overlay) {
            auto it = std::find(mLayers.begin(), mLayers.end(), overlay);
            if (it != mLayers.end()) {
                mLayers.erase(it);
            }

            auto activeIt = std::find(mActiveLayers.begin(), mActiveLayers.end(), overlay);
            if (activeIt != mActiveLayers.end()) {
                mActiveLayers.erase(activeIt);
            }
        }

        void setLayerActive(T layer, bool active = true) {
            auto it = std::find(mActiveLayers.begin(), mActiveLayers.end(), layer);
            if (active && it == mActiveLayers.end()) {
                mActiveLayers.push_back(layer);
            }
            else if (!active && it != mActiveLayers.end()) {
                mActiveLayers.erase(it);
            }
        }

        const std::vector<T>& getAllLayers() const { return mLayers; }
        const std::vector<T>& getAllActiveLayers() const { return mActiveLayers; }

        typename std::vector<T>::iterator begin() { return mActiveLayers.begin(); }
        typename std::vector<T>::iterator end() { return mActiveLayers.end(); }

    private:
        std::vector<T> mLayers;
        std::vector<T> mActiveLayers;
        unsigned int mLayerInsertIndex;
    };
}