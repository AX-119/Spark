-- OrbitingParticles.lua
-- Creates a mesmerizing particle system that orbits around the mouse cursor

Particles = {
    system = {},
    transform = nil,
    renderer = nil,
    max_particles = 50,
    spawn_rate = 0.05, -- spawn every 50ms
    spawn_timer = 0,
    attraction_strength = 200.0,
    orbit_speed = 2.0
}

function Init()
    Particles.transform = gameObject:GetTransformComponent()
    Particles.renderer = get_renderer()
end

function Update(dt)
    Particles.spawn_timer = Particles.spawn_timer + dt
    
    -- Spawn new particles
    if Particles.spawn_timer >= Particles.spawn_rate and #Particles.system < Particles.max_particles then
        spawn_particle()
        Particles.spawn_timer = 0
    end
    
    -- Update all particles
    local mouse_x, mouse_y = get_mouse_position()
    local mouse_pos = vec3(mouse_x, mouse_y, 0)
    
    for i = #Particles.system, 1, -1 do
        local particle = Particles.system[i]
        
        -- Age the particle
        particle.age = particle.age + dt
        particle.life = particle.life - dt
        
        -- Remove dead particles
        if particle.life <= 0 then
            table.remove(Particles.system, i)
        else
            -- Calculate attraction to mouse
            local to_mouse = mouse_pos - particle.pos
            local distance = to_mouse:length()
            
            if distance > 1.0 then
                -- Apply orbital motion
                local tangent = vec3(-to_mouse.y, to_mouse.x, 0):normalize()
                local attraction = to_mouse:normalize() * Particles.attraction_strength * dt
                local orbital = tangent * Particles.orbit_speed * (100.0 / (distance + 10.0))
                
                particle.velocity = particle.velocity + attraction * 0.3 + orbital
                particle.velocity = particle.velocity * 0.95 -- damping
                
                particle.pos = particle.pos + particle.velocity * dt
            end
            
            -- Update visual properties
            particle.size = particle.base_size * (particle.life / particle.max_life)
            particle.rotation = particle.rotation + particle.spin_speed * dt
        end
    end
end

function Render()
    for i, particle in ipairs(Particles.system) do
        local alpha = particle.life / particle.max_life
        local hue_shift = particle.age * 0.5
        
        -- Create rainbow effect
        local r = (math.sin(hue_shift) + 1) * 0.5
        local g = (math.sin(hue_shift + 2.1) + 1) * 0.5
        local b = (math.sin(hue_shift + 4.2) + 1) * 0.5
        
        Particles.renderer:set_draw_color_float(r * alpha, g * alpha, b * alpha, alpha)
        
        -- Draw particle as a rotating square
        draw_rotated_square(particle.pos, particle.size, particle.rotation)
    end
    
    -- Draw mouse cursor indicator
    local mouse_x, mouse_y = get_mouse_position()
    Particles.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.3)
    draw_circle(vec3(mouse_x, mouse_y, 0), 20, 12)
end

function spawn_particle()
    local mouse_x, mouse_y = get_mouse_position()
    local angle = math.random() * 2 * math.pi
    local distance = 100 + math.random() * 200
    
    local particle = {
        pos = vec3(mouse_x + math.cos(angle) * distance, mouse_y + math.sin(angle) * distance, 0),
        velocity = vec3(0, 0, 0),
        life = 3.0 + math.random() * 2.0,
        max_life = 0,
        age = 0,
        base_size = 8 + math.random() * 12,
        size = 0,
        rotation = 0,
        spin_speed = (math.random() - 0.5) * 10.0
    }
    particle.max_life = particle.life
    particle.size = particle.base_size
    
    table.insert(Particles.system, particle)
end

function draw_rotated_square(pos, size, rotation)
    local half_size = size * 0.5
    local cos_r = math.cos(rotation)
    local sin_r = math.sin(rotation)
    
    -- Calculate rotated corners
    local corners = {}
    local offsets = {{-half_size, -half_size}, {half_size, -half_size}, {half_size, half_size}, {-half_size, half_size}}
    
    for i, offset in ipairs(offsets) do
        local x = offset[1] * cos_r - offset[2] * sin_r + pos.x
        local y = offset[1] * sin_r + offset[2] * cos_r + pos.y
        corners[i] = {x, y}
    end
    
    -- Draw the square as connected lines
    for i = 1, 4 do
        local next_i = (i % 4) + 1
        Particles.renderer:render_line(corners[i][1], corners[i][2], corners[next_i][1], corners[next_i][2])
    end
end

function draw_circle(pos, radius, segments)
    for i = 0, segments - 1 do
        local angle1 = (i / segments) * 2 * math.pi
        local angle2 = ((i + 1) / segments) * 2 * math.pi
        
        local x1 = pos.x + math.cos(angle1) * radius
        local y1 = pos.y + math.sin(angle1) * radius
        local x2 = pos.x + math.cos(angle2) * radius
        local y2 = pos.y + math.sin(angle2) * radius
        
        Particles.renderer:render_line(x1, y1, x2, y2)
    end
end