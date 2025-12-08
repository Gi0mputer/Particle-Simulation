# BOIDS GPU Implementation Design Document

## Objective
Implement efficient Boids (alignment, cohesion, separation) for 1M particles at 60 FPS on 4K texture.

## Performance Constraints
- Target: 1M particles @ 60 FPS
- GPU Memory Budget: ~200MB additional
- Neighbor search radius: 50-100 pixels
- Max neighbors per particle: 32-64 (capped for performance)

## Architecture: Spatial Hashing Grid

### Why Spatial Hashing?
- O(N) construction time
- O(1) average lookup time
- GPU-friendly (parallel construction)
- Memory efficient

### Grid Specifications
**Grid Cell Size:** 80 pixels (adjustable via UI: 50-150px)
- Matches typical Boids interaction radius
- 4K texture (3840x2160) → Grid: 48x27 = 1,296 cells
- Expected particles per cell: 1M / 1,296 ≈ 772 particles/cell

**Grid Storage:**
```
GridCell {
    uint particleCount;      // Number of particles in this cell
    uint particleStartIndex; // Start index in sorted particle list
}
```

**Total cells:** 64x32 = 2,048 cells (power of 2 for alignment)
**Memory:** 2,048 cells × 8 bytes = 16 KB (negligible!)

### Particle Sorting
**Sorted Particle Buffer:**
- Particles sorted by grid cell ID
- Allows contiguous memory access per cell
- Size: 1M particles × 4 bytes (index) = 4 MB

### Memory Budget
- Grid cells: 16 KB
- Sorted indices: 4 MB
- Cell counts: 16 KB
- **Total additional memory: ~4.5 MB** ✓ (well within budget!)

## Implementation Phases

### Phase 1: Grid Data Structures (C++)
- Add grid buffers to SimulationGPU
- Grid cell buffer (SSBO)
- Particle-to-cell mapping buffer
- Cell start offset buffer

### Phase 2: Grid Construction Shader
**Compute Shader: `grid_build.comp`**
- Local size: 128
- Each thread: Calculate particle's cell ID
- Atomic increment cell counters
- Store particle index in cell

### Phase 3: Boids Logic Shader
**Integrate into `update.comp`:**
- Query neighbors from grid (3x3 cell neighborhood = 9 cells)
- Calculate Boids forces:
  - **Alignment**: Average velocity of neighbors
  - **Cohesion**: Move toward center of mass
  - **Separation**: Avoid crowding
- Apply weighted forces to particle angle/speed

### Phase 4: UI Integration
- Toggle "Active on particles"
- Sliders for alignment/separation/cohesion weights
- Slider for interaction radius

## GPU Optimization Strategies

### 1. Early Exit Optimization
```glsl
if (neighborCount == 0) return; // Skip empty cells
if (neighborCount > MAX_NEIGHBORS) neighborCount = MAX_NEIGHBORS; // Cap
```

### 2. Shared Memory Usage
```glsl
layout(local_size_x = 128) in;
shared vec2 sharedNeighborPos[128]; // Cache neighbor positions
```

### 3. Coalesced Memory Access
- Sort particles by cell ID for contiguous reads
- Process cells in grid order

### 4. Parallel Grid Construction
- Use atomic operations for thread-safe cell counting
- Two-pass: Count → Prefix Sum → Fill

## Integration with Physarum

**Hybrid Mode (Both enabled):**
```glsl
vec2 physarumForce = calculatePhysarumForce();
vec2 boidsForce = calculateBoidsForce();
vec2 totalForce = physarumForce * uPhysarumIntensity + 
                  boidsForce * uBoidsIntensity;
angle = atan(totalForce.y, totalForce.x);
```

## Performance Targets
- Grid construction: <1ms
- Neighbor queries: <2ms (1M particles, avg 32 neighbors each)
- Boids calculations: <2ms
- **Total Boids overhead: <5ms** (66% of 16ms frame budget @ 60 FPS)

## Fallback Strategies
If performance falls below 60 FPS:
1. Reduce interaction radius (fewer neighbors)
2. Cap max neighbors to 16
3. Process Boids every 2nd frame
4. Reduce particle count

## Next Steps
1. Implement grid buffers in SimulationGPU
2. Create grid_build.comp shader
3. Update update.comp with Boids logic
4. Add UI controls
5. Profile and optimize
