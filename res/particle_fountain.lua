-- ParticleFountain.lua
-- A particle system with fountains, wind effects, and interactive elements

ParticleFountain = {
    transform = nil,
    renderer = nil,
    particles = {},
    emitters = {},
    max_particles = 200,
    gravity = vec3(0, -80, 0),
    wind = vec3(0, 0, 0),
    time = 0,
    mouse_force_strength = 500.0,
    particle_lifetime = 5.0
}

function Init()
    ParticleFountain.transform = gameObject:GetTransformComponent()
    ParticleFountain.renderer = get_renderer()
    
    -- Create initial emitters
    local window = get_window()
    create_emitter(window.width * 0.25, window.height - 50, "fountain")
    create_emitter(window.width * 0.75, window.height - 50, "fountain")
    create_emitter(window.width * 0.5, 50, "rain")
end

function Update(dt)
    ParticleFountain.time = ParticleFountain.time + dt
    
    -- Handle input
    handle_input(dt)
    
    -- Update wind (sinusoidal pattern)
    ParticleFountain.wind.x = math.sin(ParticleFountain.time * 0.5) * 30
    ParticleFountain.wind.y = math.cos(ParticleFountain.time * 0.3) * 10
    
    -- Update emitters
    update_emitters(dt)
    
    -- Update particles
    update_particles(dt)
    
    -- Clean up old particles
    cleanup_particles()
end

function Render()
    -- Draw particles
    draw_particles()
    
    -- Draw emitters
    draw_emitters()
    
    -- Draw mouse influence
    draw_mouse_influence()
    
    -- Draw wind indicator
    draw_wind_indicator()
    
    -- Draw UI
    draw_ui()
end

function handle_input(dt)
    -- Left click: Create temporary emitter
    if is_left_mouse_down() then
        local mouse_x, mouse_y = get_mouse_position()
        emit_particles_at(mouse_x, mouse_y, 3, "burst")
    end
    
    -- Right click: Wind boost
    -- Middle click handled in particle physics
end

function create_emitter(x, y, type)
    local emitter = {
        pos = vec3(x, y, 0),
        type = type,
        rate = 0,
        timer = 0,
        active = true
    }
    
    if type == "fountain" then
        emitter.rate = 15 -- particles per second
    elseif type == "rain" then
        emitter.rate = 8
    elseif type == "burst" then
        emitter.rate = 50
        emitter.lifetime = 0.5
    end
    
    table.insert(ParticleFountain.emitters, emitter)
end

function update_emitters(dt)
    for i = #ParticleFountain.emitters, 1, -1 do
        local emitter = ParticleFountain.emitters[i]
        
        if emitter.active then
            emitter.timer = emitter.timer + dt
            
            -- Emit particles based on rate
            local particles_to_emit = math.floor(emitter.timer * emitter.rate)
            if particles_to_emit > 0 then
                emit_particles_at(emitter.pos.x, emitter.pos.y, particles_to_emit, emitter.type)
                emitter.timer = emitter.timer - (particles_to_emit / emitter.rate)
            end
            
            -- Handle burst emitters
            if emitter.type == "burst" then
                emitter.lifetime = emitter.lifetime - dt
                if emitter.lifetime <= 0 then
                    table.remove(ParticleFountain.emitters, i)
                end
            end
        end
    end
end

function emit_particles_at(x, y, count, type)
    for i = 1, count do
        if #ParticleFountain.particles < ParticleFountain.max_particles then
            local particle = create_particle(x, y, type)
            table.insert(ParticleFountain.particles, particle)
        end
    end
end

function create_particle(x, y, type)
    local particle = {
        pos = vec3(x, y, 0),
        velocity = vec3(0, 0, 0),
        life = ParticleFountain.particle_lifetime,
        max_life = ParticleFountain.particle_lifetime,
        size = 2 + math.random() * 4,
        color = {r = 1, g = 1, b = 1},
        type = type
    }
    
    -- Set initial velocity based on type
    if type == "fountain" then
        local angle = -math.pi/2 + (math.random() - 0.5) * math.pi/3
        local speed = 100 + math.random() * 50
        particle.velocity.x = math.cos(angle) * speed
        particle.velocity.y = math.sin(angle) * speed
        particle.color = {r = 0.3 + math.random() * 0.7, g = 0.5 + math.random() * 0.5, b = 1.0}
    elseif type == "rain" then
        particle.velocity.x = (math.random() - 0.5) * 20
        particle.velocity.y = 50 + math.random() * 30
        particle.color = {r = 0.7, g = 0.8, b = 1.0}
        particle.size = 1 + math.random() * 2
    elseif type == "burst" then
        local angle = math.random() * 2 * math.pi
        local speed = 50 + math.random() * 100
        particle.velocity.x = math.cos(angle) * speed
        particle.velocity.y = math.sin(angle) * speed
        particle.color = {r = 1.0, g = 0.5 + math.random() * 0.5, b = 0.2}
        particle.life = 1.0 + math.random() * 2.0
        particle.max_life = particle.life
    end
    
    return particle
end

function update_particles(dt)
    local mouse_x, mouse_y = get_mouse_position()
    local mouse_pos = vec3(mouse_x, mouse_y, 0)
    
    for i, particle in ipairs(ParticleFountain.particles) do
        -- Apply gravity
        particle.velocity = particle.velocity + ParticleFountain.gravity * dt
        
        -- Apply wind
        particle.velocity = particle.velocity + ParticleFountain.wind * dt
        
        -- Mouse interaction
        if is_right_mouse_down() then
            local to_mouse = mouse_pos - particle.pos
            local distance = to_mouse:length()
            if distance > 1.0 and distance < 150.0 then
                local force = to_mouse:normalize() * (ParticleFountain.mouse_force_strength / (distance * 0.1))
                particle.velocity = particle.velocity + force * dt
            end
        elseif is_middle_mouse_down() then
            local from_mouse = particle.pos - mouse_pos
            local distance = from_mouse:length()
            if distance > 1.0 and distance < 150.0 then
                local force = from_mouse:normalize() * (ParticleFountain.mouse_force_strength / (distance * 0.1))
                particle.velocity = particle.velocity + force * dt
            end
        end
        
        -- Update position
        particle.pos = particle.pos + particle.velocity * dt
        
        -- Update life
        particle.life = particle.life - dt
        
        -- Simple collision with ground
        local window = get_window()
        if particle.pos.y > window.height - 10 then
            particle.pos.y = window.height - 10
            particle.velocity.y = particle.velocity.y * -0.3 -- Bounce with energy loss
            particle.velocity.x = particle.velocity.x * 0.8 -- Friction
        end
        
        -- Bounce off walls
        if particle.pos.x < 0 then
            particle.pos.x = 0
            particle.velocity.x = math.abs(particle.velocity.x) * 0.7
        elseif particle.pos.x > window.width then
            particle.pos.x = window.width
            particle.velocity.x = -math.abs(particle.velocity.x) * 0.7
        end
    end
end

function cleanup_particles()
    for i = #ParticleFountain.particles, 1, -1 do
        local particle = ParticleFountain.particles[i]
        if particle.life <= 0 then
            table.remove(ParticleFountain.particles, i)
        end
    end
end

function draw_particles()
    for i, particle in ipairs(ParticleFountain.particles) do
        local life_ratio = particle.life / particle.max_life
        local alpha = life_ratio * 0.8
        
        -- Fade color over lifetime
        local r = particle.color.r * life_ratio
        local g = particle.color.g * life_ratio
        local b = particle.color.b * life_ratio
        
        ParticleFountain.renderer:set_draw_color_float(r, g, b, alpha)
        
        -- Draw particle as small filled circle
        draw_filled_circle(particle.pos, particle.size * life_ratio, 6)
        
        -- Draw velocity trail for fast particles
        local speed = particle.velocity:length()
        if speed > 50 then
            ParticleFountain.renderer:set_draw_color_float(r, g, b, alpha * 0.3)
            local trail_end = particle.pos - particle.velocity:normalize() * (speed * 0.05)
            ParticleFountain.renderer:render_line(particle.pos.x, particle.pos.y, trail_end.x, trail_end.y)
        end
    end
end

function draw_emitters()
    for i, emitter in ipairs(ParticleFountain.emitters) do
        if emitter.active then
            local pulse = 0.5 + math.sin(ParticleFountain.time * 5 + i) * 0.5
            
            if emitter.type == "fountain" then
                ParticleFountain.renderer:set_draw_color_float(0.2, 0.5, 1.0, pulse * 0.8)
            elseif emitter.type == "rain" then
                ParticleFountain.renderer:set_draw_color_float(0.5, 0.7, 1.0, pulse * 0.6)
            end
            
            draw_circle(emitter.pos, 8 + pulse * 4, 8)
        end
    end
end

function draw_mouse_influence()
    local mouse_x, mouse_y = get_mouse_position()
    
    if is_right_mouse_down() then
        ParticleFountain.renderer:set_draw_color_float(0.2, 1.0, 0.2, 0.4)
        for radius = 30, 120, 30 do
            draw_circle(vec3(mouse_x, mouse_y, 0), radius, 12)
        end
    elseif is_middle_mouse_down() then
        ParticleFountain.renderer:set_draw_color_float(1.0, 0.2, 0.2, 0.4)
        for radius = 30, 120, 30 do
            draw_circle(vec3(mouse_x, mouse_y, 0), radius, 12)
        end
    end
end

function draw_wind_indicator()
    local window = get_window()
    local wind_strength = ParticleFountain.wind:length()
    
    if wind_strength > 5 then
        ParticleFountain.renderer:set_draw_color_float(1.0, 1.0, 0.5, 0.6)
        
        -- Draw wind arrow in top-right corner
        local base_x, base_y = window.width - 60, 40
        local wind_normalized = ParticleFountain.wind:normalize()
        local arrow_length = math.min(wind_strength * 0.8, 40)
        
        local end_x = base_x + wind_normalized.x * arrow_length
        local end_y = base_y + wind_normalized.y * arrow_length
        
        ParticleFountain.renderer:render_line(base_x, base_y, end_x, end_y)
        
        -- Arrow head
        local head_size = 8
        local perp_x = -wind_normalized.y * head_size
        local perp_y = wind_normalized.x * head_size
        
        ParticleFountain.renderer:render_line(end_x, end_y, end_x - wind_normalized.x * head_size + perp_x, end_y - wind_normalized.y * head_size + perp_y)
        ParticleFountain.renderer:render_line(end_x, end_y, end_x - wind_normalized.x * head_size - perp_x, end_y - wind_normalized.y * head_size - perp_y)
    end
end

function draw_ui()
    local window = get_window()
    
    ParticleFountain.renderer:set_draw_color_float(1.0, 1.0, 1.0, 0.8)
    
    -- Particle count bar
    local particle_ratio = #ParticleFountain.particles / ParticleFountain.max_particles
    local bar_width = particle_ratio * 100
    for i = 0, 2 do
        ParticleFountain.renderer:render_line(10, 10 + i, 10 + bar_width, 10 + i)
    end
    
    -- Emitter count indicator
    local emitter_count = #ParticleFountain.emitters
    for i = 1, emitter_count do
        ParticleFountain.renderer:render_line(10 + i * 8, 20, 10 + i * 8, 25)
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
        
        ParticleFountain.renderer:render_line(x1, y1, x2, y2)
    end
end

function draw_filled_circle(pos, radius, segments)
    for r = 1, radius, 1 do
        draw_circle(pos, r, segments)
    end
end