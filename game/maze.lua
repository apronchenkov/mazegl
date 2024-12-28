require("tracer")

function level_1()
  local tracer = tracer.Tracer:new()
  tracer:set_entrance()
  tracer:push()
  tracer:go(10)
  tracer:pop()
  tracer:turn_left()
  tracer:push()
  tracer:go(10)
  tracer:pop()
  tracer:turn_left()
  tracer:push()
  tracer:go(10)
  tracer:pop()
  tracer:turn_left()
  tracer:push()
  tracer:go(10)
  tracer:pop()
  tracer:go(3)
  tracer:set_exit()
  return tracer:_render()
end


local n = 1

function next_game_map(suggest_width, suggest_height)
  if n == 1 then
    n = 2
    return level_1()
  elseif n == 2 then
    n = 1
    return "#+#\n...\n#-#"
  end
end
