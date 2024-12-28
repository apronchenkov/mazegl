require("tracer")

function level_1()
  local t = tracer.Tracer:new()
  t:set_entrance()
  t:push()
  t:go(10)
  t:pop()
  t:turn_left()
  t:push()
  t:go(10)
  t:pop()
  t:turn_left()
  t:push()
  t:go(10)
  t:pop()
  t:turn_left()
  t:push()
  t:go(10)
  t:pop()
  t:go(3)
  t:set_exit()
  return t:_render()
end


function level_2(n)
  local t = tracer.Tracer:new()

  function impl(a, b, n)
    if n >= 5 then
      local k = (n - 1) / 2
      a(t)
      impl(b, a, k)
      a(t)
      t:go(2)
      impl(a, b, k)
      b(t)
      t:go(2)
      b(t)
      impl(a, b, k)
      t:go(2)
      a(t)
      impl(b, a, k)
      a(t)
    else
      t:go(n)
      a(t)
      t:go(n)
      a(t)
      t:go(n)
    end
  end

  impl(t.turn_right, t.turn_left, n)
  t:set_exit()
  return t:_render()
end


print(level_2(55))

-- local n = 0
-- function next_game_map(suggest_width, suggest_height)
--   n = n + 1
--   if n == 1 then
--     return level_1()
--   elseif n == 2 then
--     return level_2(6)
--   elseif n == 3 then
--     return level_2(13)
--   elseif n == 4 then
--     return level_2(27)
--   elseif n == 5 then
--     return level_2(55)
--   else
--     n = 0
--     return "#+#\n...\n#-#"
--   end
-- end

local n = 2

function next_game_map(suggest_width, suggest_height)
  local result = level_2(n)
  n = 2 * n + 1
  return result
end
