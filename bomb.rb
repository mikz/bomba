require 'forwardable'

class Bomb
  SECOND = 1.to_f

  def initialize(display, timer)
    @display = display
    @timer = timer.to_i
    @speed = 1
  end

  def tick
    @display.show(@timer, pause)
    @timer -= 1
  end

  def pause
    SECOND / @speed
  end
end

# 10m = 600s = 10:00:00
# 9m 10s 11m = 9 * 60 * 100 + 10 * 100 + 11


class Display
  def initialize
    @output = SevenSegmentOutput.new
  end

  def show(time, duration)
    seconds = time % 60
    minutes = time / 60

    @output.display(minutes, seconds, duration)
  end
end


class Output
  def display(minutes, seconds, duration)
    raise 'reimplement'
  end
end

class ConsoleOutput < Output
  def display(*parts, duration)
    puts parts.flatten.map{ |t| t.to_s.rjust(2, '0') }.join(':')
    sleep(duration)
  end
end

class SegmentConsoleOutput < Output
  def display(minutes, seconds, duration)
    print "\r#{minutes}:#{seconds}"
    sleep duration
  end
end

class SegmentDisplay
  SEGMENTS = 4

  def initialize
    @segments = SEGMENTS.times.to_a
    @values = Array.new(SEGMENTS)
  end

  def []=(segment, value)
    @values[segment] = value
  end

  def refresh
    rotate
    output
  end

  def current_value
    @values[active]
  end

  def output
    pre = ' ' * active
    post = ' ' * (SEGMENTS - active - 1)
    print "\r#{pre}#{current_value}#{post}"
  end

  def active
    @segments.first
  end

  def rotate
    @segments.rotate!
  end
end

class SevenSegmentOutput < Output
  def initialize
    @display = SegmentDisplay.new
    @rate = 30
  end

  def display(minutes, seconds, duration)

    @display[0] = minutes / 10
    @display[1] = minutes % 10
    @display[2] = seconds / 10
    @display[3] = seconds % 10

    @rate.times do
      pause = duration / @rate.to_f
      sleep(pause)
      @display.refresh
    end
  end
end
