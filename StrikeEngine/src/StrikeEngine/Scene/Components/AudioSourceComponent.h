#pragma once
#include "Component.h"
#include <string>
#include <memory>
#include <vector>
#include <pugixml.hpp>

namespace Strike {
    class Audio;

    /**
     * @brief Component for playing audio clips with 3D spatial audio support.
     * 
     * The AudioSourceComponent manages audio playback with control over volume,
     * looping, spatial audio settings, and distance attenuation parameters.
     * 
     * @note Spatial audio requires an active AudioListenerComponent in the scene
     */
    class AudioSourceComponent : public Component {
    public:
        /**
         * @brief Default constructor
         */
        AudioSourceComponent();

        /**
         * @brief Get the static type name for this component
         * 
         * @return Constant reference to the static type name "audiosource"
         */
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "audiosource";
            return typeName;
        }

        /**
         * @brief Get the type name of this component instance
         * 
         * @return Constant reference to the type name "audiosource"
         */
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        /**
         * @brief Deserialize audio source data from XML node
         * 
         * @param node The XML node containing audio source configuration
         */
        void deserialize(const pugi::xml_node& node) override;

        /**
         * @brief Set the audio clip to be played
         * 
         * @param audioId The asset ID of the audio clip
         */
        void setAudio(const std::string& audioId);
        
        /**
         * @brief Remove the currently assigned audio clip
         */
        void removeAudio();
        
        /**
         * @brief Get the ID of the currently assigned audio clip
         * 
         * @return Constant reference to the audio asset ID
         */
        const std::string& getAudioId() const { return mAudioId; }
        
        /**
         * @brief Get the audio resource object
         * 
         * @return Shared pointer to the Audio object, or nullptr if not set
         */
        std::shared_ptr<Audio> getAudio() const;
        
        /**
         * @brief Check if an audio clip is assigned
         * 
         * @return True if an audio clip is assigned, false otherwise
         */
        bool hasAudio() const;

        /**
         * @brief Start playing the audio clip
         * 
         * @note Playback will begin on the next audio system update
         */
        void play(); 
        
        /**
         * @brief Stop playing the audio clip
         * 
         * @note Playback will stop on the next audio system update
         */
        void stop(); 
    
        /**
         * @brief Set the playback volume
         * 
         * @param volume Volume level (0.0 to 1.0+, where 1.0 is full volume)
         */
        void setVolume(float volume);
        
        /**
         * @brief Get the current playback volume
         * 
         * @return Current volume level
         */
        float getVolume() const { return mVolume; }

        /**
         * @brief Enable or disable audio looping
         * 
         * @param loop True to loop the audio, false to play once
         */
        void setLoop(bool loop) { mLoop = loop; }
        
        /**
         * @brief Check if audio looping is enabled
         * 
         * @return True if looping is enabled, false otherwise
         */
        bool isLoop() const { return mLoop; }

        /**
         * @brief Enable or disable 3D spatial audio
         * 
         * @param spatial True for 3D spatial audio, false for 2D audio
         * @note Spatial audio is affected by distance and position relative to the AudioListener
         */
        void setSpatial(bool spatial) { mSpatial = spatial; }
        
        /**
         * @brief Check if spatial audio is enabled
         * 
         * @return True if spatial audio is enabled, false otherwise
         */
        bool isSpatial() const { return mSpatial; }

        /**
         * @brief Set the minimum distance for distance attenuation
         * 
         * @param distance Minimum distance in world units
         * @note Audio plays at full volume within this distance
         */
        void setMinDistance(float distance) { minDistance = distance; }
        
        /**
         * @brief Get the minimum distance for distance attenuation
         * 
         * @return Minimum distance in world units
         */
        float getMinDistance() const { return minDistance; }

        /**
         * @brief Set the maximum distance for distance attenuation
         * 
         * @param distance Maximum distance in world units
         * @note Audio volume reaches zero at this distance
         */
        void setMaxDistance(float distance) { maxDistance = distance; }
        
        /**
         * @brief Get the maximum distance for distance attenuation
         * 
         * @return Maximum distance in world units
         */
        float getMaxDistance() const { return maxDistance; }

    private:
        friend class AudioSystem;

        std::string mAudioId;              ///< Asset ID of the audio clip
        float mVolume = 1.0f;              ///< Playback volume (0.0 to 1.0+)
        bool mLoop = false;                ///< Loop playback flag
        bool mSpatial = false;             ///< Spatial audio flag

        float minDistance = 1.0f;          ///< Minimum distance for attenuation
        float maxDistance = 50.0f;         ///< Maximum distance for attenuation

        bool mPlayRequested = false;       ///< Internal play request flag
        bool mStopRequested = false;       ///< Internal stop request flag
    };
}
