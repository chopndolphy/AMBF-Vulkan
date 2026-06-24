# FIRE-3DV — Framework-Independent Rendering Engine for 3D Graphics using Vulkan

A from-scratch, real-time **Vulkan** rendering engine built to modernize the legacy OpenGL pipelines used in robotics and surgical simulation. It renders a **7-million-triangle scene in under 2 ms of GPU time on an RTX 4090** while delivering physically based rendering, anti-aliasing, and **hardware ray-traced shadows** — and it plugs into existing simulators through a lightweight shared-memory interface.

This work was published at **[IEEE IRC 2024](https://ieeexplore.ieee.org/document/10818107)** (preprint: [arXiv:2410.05095](https://arxiv.org/abs/2410.05095)) and developed in collaboration with Johns Hopkins University and WPI.

📄 **[Read the paper (PDF)](fire3dv.pdf)** · 🌐 **[Project page](https://fire-3dv-repositories.github.io/towards-modern-drs-engine/)** · 🎓 **[IEEE Xplore](https://ieeexplore.ieee.org/document/10818107)**

![FIRE-3DV rendering a da Vinci surgical robot with PBR materials and ray-traced shadows](fire3dv.png "FIRE-3DV")

---

## The problem

Interactive robotics simulators like the [Asynchronous Multi-Body Framework (AMBF)](https://github.com/WPI-AIM/ambf) are essential for training surgeons and generating data for machine learning — but their visual fidelity is capped by **legacy OpenGL 2.1 (released in 2006)**. That API predates programmable shaders done right, GPU compute, and hardware ray tracing, and Khronos stopped developing OpenGL entirely in 2017. Simulators stuck on it can never access modern GPU features, no matter how good the hardware gets.

FIRE-3DV is the answer: a **standalone, framework-independent renderer on Vulkan** (OpenGL's successor) that any simulator can adopt with minimal changes — keeping its physics, input devices, and plugin architecture while gaining a modern graphics pipeline.

## What it does

- **Physically based rendering (PBR)** — Cook-Torrance specular model (Trowbridge-Reitz GGX + Schlick), compatible with the glTF metallic-roughness workflow, so real-world material measurements render accurately.
- **Hardware ray-traced shadows** — dynamic, omnidirectional hard shadows via Vulkan's ray-query extension, with a bottom/top-level acceleration-structure build system offloaded to an async compute queue.
- **Anti-aliasing** — combines MSAA (in the main pass) and FXAA (as a post-process) to cut edge and specular aliasing at low cost.
- **Framework-independent integration** — a simulator writes object transforms into shared memory each frame (via `boost::interprocess`); FIRE-3DV reads them and renders. Demonstrated by modernizing AMBF, but designed to drop into any engine.
- **Modern Vulkan architecture** — vertex pulling, draw calls sorted by material and vertex buffer to minimize descriptor binds and cache misses, lambda-based deletion queues for resource lifetimes, a double-buffered swapchain, and a multi-pass loop (main → post-process → Dear ImGui → present).

> 🚧 **In progress:** a full hardware ray-tracing pipeline (ray-generation, closest-hit, and miss shaders) is under development on the [`raytracing` branch](https://github.com/chopndolphy/FIRE-3DV/tree/raytracing), extending beyond the ray-traced shadows in the published version toward fully ray-traced rendering.

## Results

Measured on an Ubuntu 22.04 workstation with an NVIDIA RTX 4090, profiled with NVIDIA Nsight Graphics:

| Metric | Result |
| --- | --- |
| GPU time, 7M+ triangle scene | **< 2 ms** |
| Main-pass speedup vs. AMBF (legacy OpenGL) | **6.1× – 8.5×** |
| Features | PBR · MSAA + FXAA · ray-traced shadows |

Because rendering is so fast, the full simulation becomes CPU-bound rather than GPU-bound — leaving headroom for higher-fidelity effects.

## Tech stack

**C++20** · **Vulkan 1.3** · GLSL · CMake
Libraries: [SDL2](https://www.libsdl.org/) (windowing) · [Dear ImGui](https://github.com/ocornut/imgui) (GUI) · [fastgltf](https://github.com/spnda/fastgltf) (asset loading) · [GLM](https://github.com/g-truc/glm) (math) · [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndCommunity/VulkanMemoryAllocator) · [vk-bootstrap](https://github.com/charles-lunarg/vk-bootstrap) · [volk](https://github.com/zeux/volk) · [stb_image](https://github.com/nothings/stb) · [Boost.Interprocess](https://www.boost.org/)

## What I learned

This was my deepest dive into low-level graphics: managing GPU memory and synchronization by hand, designing a render-pass graph, implementing a PBR lighting model from the published equations, and building a ray-tracing acceleration-structure pipeline from scratch. Just as valuable was the systems-integration work — bridging two independent processes through shared memory without the concurrency pitfalls — and the experience of writing it all up into a peer-reviewed publication.

---

## Build & run

> **Platform:** Linux (developed and tested on Ubuntu 22.04, X11). Requires a Vulkan 1.3-capable GPU; ray-traced shadows require ray-tracing hardware support.

<details>
<summary><b>1. Install Vulkan SDK</b></summary>

```bash
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
sudo apt update
sudo apt install vulkan-sdk vulkan-validationlayers-dev spirv-tools
```

Verify it works:

```bash
vkcube
vulkaninfo --summary
```
</details>

<details>
<summary><b>2. Install SDL2</b></summary>

```bash
sudo apt-get install libsdl2-dev
```
</details>

<details>
<summary><b>3. Get sample assets</b></summary>

Download glTF assets from the [sample assets repo](https://github.com/FIRE-3DV-repositories/src_sample_assets/tree/main/glTF) and place them in a new `assets/` folder in the project root.
</details>

<details>
<summary><b>4. Compile shaders</b></summary>

Install `glslc` ([download](https://github.com/google/shaderc/blob/main/downloads.md)) to `/usr/local/bin`, then from the `build/` directory:

```bash
glslc ../shaders/gradient_color.comp --target-env=vulkan1.3 -O -o ../shaders/gradient_color.comp.spv
glslc ../shaders/sky.comp            --target-env=vulkan1.3 -O -o ../shaders/sky.comp.spv
glslc ../shaders/pbr.frag            --target-env=vulkan1.3 -O -o ../shaders/pbr.frag.spv
glslc ../shaders/pbr.vert            --target-env=vulkan1.3 -O -o ../shaders/pbr.vert.spv
glslc ../shaders/post_process.vert   --target-env=vulkan1.3 -O -o ../shaders/post_process.vert.spv
glslc ../shaders/post_process.frag   --target-env=vulkan1.3 -O -o ../shaders/post_process.frag.spv
```
</details>

<details>
<summary><b>5. Build & run</b></summary>

From the project root:

```bash
mkdir build && cd build
cmake ..
make
./FIRE3D
```
</details>

## Citation

```bibtex
@inproceedings{allison2024fire3dv,
  title     = {FIRE-3DV: Framework-Independent Rendering Engine for 3D Graphics using Vulkan},
  author    = {Allison, Christopher John and Zhou, Haoying and Munawar, Adnan and Kazanzides, Peter and Barragan, Juan Antonio},
  booktitle = {2024 Eighth IEEE International Conference on Robotic Computing (IRC)},
  pages     = {104--111},
  year      = {2024},
  doi       = {10.1109/IRC63610.2024.00053}
}
```

## License

Released under the [MIT License](LICENSE).

## Acknowledgments

Supported in part by NSF AccelNet awards OISE-1927354 and OISE-1927275. Developed at Northeastern University in collaboration with Johns Hopkins University and Worcester Polytechnic Institute.
