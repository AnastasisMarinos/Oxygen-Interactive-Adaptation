# OXYGEN — Dynamic Narrative Audio-Visual System (Unreal Engine)

**OXYGEN** is an experimental narrative experience built in **Unreal Engine**, designed to explore **dynamic audio–visual synchronization, spatial interaction, and data-driven storytelling**.
Inspired by the rhythm, pacing, and thematic structure of Ivan Vyrypaev’s *Oxygen*, this project reimagines a section of the text as an **interactive moving scene**, where lighting, post-processing, and sound respond directly to player-driven narrative progression.

All systems are implemented using **native Unreal Engine functionality** — no third-party plugins, assets, or middleware.

---

## ⭐ Overview

At its core, OXYGEN is a **modular, beat-aware narrative engine**.
The environment reacts dynamically to:

* VO playback
* Narrative themes
* Lighting snapshots
* Post-process transitions
* Beat-synced spawning

This creates a **tightly choreographed experience** where music, lighting, and narrative operate in sync, forming a unified theatrical atmosphere.

---

## 🎛 Dynamic Audio–Visual Pipeline

### **Audio Snapshot System**

* Dedicated audio profiles per line of narration
* Built-in Unreal features:

  * Submix Dynamics Processor
  * EQ adjustments
  * Reverb sends
  * Output gain automation
  * Timed fades & crossfades
* Snapshots update instantly or smoothly (200–400ms) for drama-appropriate transitions:

  * **Celestial:** bright shimmering highs
  * **Conflict:** tight, dry, high-impact mix
  * **Mourning:** long plate tails, desaturation
  * **Vice/Transgression:** subtle distortion, dimmed ambience
* **No external audio middleware** used

### **Lighting & Post-Process Snapshots**

* Each narrative theme drives scene-wide mood changes:

  * Color grading
  * Exposure
  * Light color/intensity
  * Pulse/strobe variations
  * Vignette for reflection sections
* Snapshots are blended via built-in Unreal curves for **smooth, theatrical transitions**

### **Beat-Sync Clock**

* Internal BPM-derived clock ensures:

  * Line spawns align to musical downbeats
  * Auto-advance uses bar counts
  * Visual pulses match the underlying rhythm
* Creates a **concert-like timing structure** while remaining fully interactive

---

## 🧩 Core Systems

### **Word Station System**

* Word Stations act as symbolic, in-world narrative triggers
* Features:

  * Rise-from-floor and lower animations
  * Screen-space interaction prompts
  * Dynamic spawn locations
  * Player proximity detection
  * Auto-advance fail-safe
  * Line-linked keyword display
  * Data-driven behavior
* Each station triggers:

  * VO line playback
  * Lighting + post-process snapshot
  * Music profile update
  * Spawn of next station
* Driven entirely by narrative data

### **Narrative Sequencer (AAudioManager)**

* Orchestrates the experience as the central director component
* Responsibilities:

  * Sequential VO playback
  * Subtitle scheduling
  * A/V snapshot application
  * Theme transition tracking
  * Auto-advance handling
  * BPM alignment
  * Event broadcasting for other systems
* Designed to be **modular, predictable, and artist-friendly**

### **Spatial Design**

* Single, crowded underground club environment
* Features:

  * Clear anchor points for narrative progression
  * Emissive, high-contrast keyword visibility

---

## 🎨 Visual Identity

* Heavy contrast between dark ambience and bright keywords
* High-emissive text as symbolic objects
* Atmospheric fog and colored washes
* Dynamic mood swings controlled entirely by data
* Minimal UI — the world itself carries the storytelling
* Hybrid of **theatre, concert staging, and interactive installation**

---

## 🎙 Narrative Foundation

* Theatrical adaptation of material from Ivan Vyrypaev’s *Oxygen*
* Focus on rhythm, contradiction, and thematic escalation
* Text used strictly for **educational and portfolio purposes**

---

## 🔧 Technical Highlights

* 100% **C++ foundation** with lightweight Blueprints for UI
* No third-party plugins
* Deferred spawning patterns
* Timer-driven event architecture
* Data-driven narrative structure
* Snapshot blending across multiple subsystems
* Clean event delegation for expandability

---

## 📌 Goals

This project explores:

* The limits of **Unreal’s built-in A/V systems**
* Narrative pacing controlled by **player traversal**
* Intersection of **interactive media and theatre**
* High responsiveness **without external middleware**
* Smooth, art-directed transitions using **real-time snapshot systems**

It is designed as a **technical showcase and creative experiment**, rather than a commercial game.
