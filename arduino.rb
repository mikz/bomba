require_relative 'bomb'
require_relative 'interval'

interval = Interval.new(minutes: 1, seconds: 5)

display = Display.new
bomba = Bomb.new(display, interval.to_seconds)

loop do
  bomba.tick
end
