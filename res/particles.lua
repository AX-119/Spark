-- ParticleExplosion.lua
ParticleSystem = {
    particles = {},
    transform = nil,
    renderer = nil,
    explosion_force = 800.0
}

function Init()
    ParticleSystem.transform = gameObject:GetTransformComponent()
    ParticleSystem.renderer = get_renderer()
end

function Update(dt)
    -- Create explosion on left click
    if is_left_mouse_down() then
		local mouse_x, mouse_y = get_mouse_position()
        local pos = vec3(mouse_x, mouse_y,0)
        for i=1, 50 do
            local angle = math.random() * math.pi*2
            local speed = math.random(50, 200)
            local particle = {
                pos = vec3(pos.x, pos.y, 0),
                vel = vec3(math.cos(angle)*speed, math.sin(angle)*speed, 0),
                life = 1.0,
                color = {math.random(), math.random(), math.random(), 1.0}
            }
            table.insert(ParticleSystem.particles, particle)
        end
    end
    
    -- Update particles
    for i=#ParticleSystem.particles, 1, -1 do
        local p = ParticleSystem.particles[i]
        p.vel = p.vel * 0.95 -- Air resistance
        p.pos = p.pos + p.vel * dt
        p.life = p.life - dt
        
        if p.life <= 0 then
            table.remove(ParticleSystem.particles, i)
        end
    end
end

function Render()
    for _, p in ipairs(ParticleSystem.particles) do
        local alpha = math.max(0, p.life)
        ParticleSystem.renderer:set_draw_color_float(
            p.color[1], p.color[2], p.color[3], alpha
        )
        local size = 10 * p.life
        local rect = create_frect(p.pos.x - size/2, p.pos.y - size/2, size, size)
        ParticleSystem.renderer:render_fill_rect(rect)
    end
end