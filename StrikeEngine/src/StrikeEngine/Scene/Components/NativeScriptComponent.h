#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/ScriptableEntity.h"
#include <vector>
#include <algorithm>
#include <type_traits>

namespace StrikeEngine {
    class NativeScriptComponent {
    public:
        NativeScriptComponent() = default;

        ~NativeScriptComponent() {
            for (auto script : m_Scripts) {
                script->OnScriptDestroy();
                delete script;  
            }
        }

        NativeScriptComponent(const NativeScriptComponent&) = delete;
        NativeScriptComponent& operator=(const NativeScriptComponent&) = delete;

        template<typename T, typename... Args>
        T& AddScript(Args&&... args) {
            static_assert(std::is_base_of<ScriptableEntity, T>::value, "Script must inherit from ScriptableEntity!");

            T* script = new T(std::forward<Args>(args)...);  
            T& scriptRef = *script;
            m_Scripts.push_back(script); 

            return scriptRef;
        }

        template<typename T>
        T* GetScript() {
            for (const auto& script : m_Scripts) {
                if (T* result = dynamic_cast<T*>(script)) {
                    return result;  
                }
            }
            return nullptr; 
        }

        template<typename T>
        void RemoveScript() {
            m_Scripts.erase(
                std::remove_if(m_Scripts.begin(), m_Scripts.end(),
                    [](ScriptableEntity* script) {
                        return dynamic_cast<T*>(script) != nullptr; 
                    }
                ),
                m_Scripts.end()
            );
        }

    private:
        const std::vector<ScriptableEntity*>& GetAllScripts() const {
            return m_Scripts; 
        }

        std::vector<ScriptableEntity*> m_Scripts; 
        friend class Scene; 
    };
}
