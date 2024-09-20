#pragma once

#include "StrikeEngine/Core/Core.h"
#include "Layer.h"

#include <vector>
#include <algorithm>

namespace StrikeEngine {

    template<typename T>
    class STRIKE_API LayerStack
    {
    public:
        LayerStack()
            : m_LayerInsertIndex(0)
        {
        }

        ~LayerStack()
        {
        }

        void PushLayer(T layer, bool active = true)
        {
            m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
            m_LayerInsertIndex++;
            if (active) {
                m_ActiveLayers.push_back(layer);
            }
        }

        void PushOverlay(T overlay, bool active = true)
        {
            m_Layers.emplace_back(overlay);
            if (active) {
                m_ActiveLayers.push_back(overlay);
            }
        }

        void PopLayer(T layer)
        {
            auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
            if (it != m_Layers.end()) {
                m_Layers.erase(it);
                m_LayerInsertIndex--;
            }

            auto activeIt = std::find(m_ActiveLayers.begin(), m_ActiveLayers.end(), layer);
            if (activeIt != m_ActiveLayers.end()) {
                m_ActiveLayers.erase(activeIt);
            }
        }

        void PopOverlay(T overlay)
        {
            auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
            if (it != m_Layers.end()) {
                m_Layers.erase(it);
            }

            auto activeIt = std::find(m_ActiveLayers.begin(), m_ActiveLayers.end(), overlay);
            if (activeIt != m_ActiveLayers.end()) {
                m_ActiveLayers.erase(activeIt);
            }
        }

        void SetLayerActive(T layer, bool active = true)
        {
            auto it = std::find(m_ActiveLayers.begin(), m_ActiveLayers.end(), layer);
            if (active && it == m_ActiveLayers.end()) {
                m_ActiveLayers.push_back(layer);
            }
            else if (!active && it != m_ActiveLayers.end()) {
                m_ActiveLayers.erase(it);
            }
        }

        const std::vector<T>& GetAllLayers() const { return m_Layers; }

        typename std::vector<T>::iterator begin() { return m_ActiveLayers.begin(); }
        typename std::vector<T>::iterator end() { return m_ActiveLayers.end(); }

    private:
        std::vector<T> m_Layers;
        std::vector<T> m_ActiveLayers;
        unsigned int m_LayerInsertIndex;
    };

}