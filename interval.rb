class Interval
  attr_reader :minutes, :seconds, :milisec

  def initialize(minutes: 0, seconds: 0, miliseconds: 0)
    @minutes = minutes
    @seconds = seconds
    @milisec = miliseconds
  end

  def to_i
    @milisec + @seconds * 100 + @minutes * 60 * 100
  end

  def to_seconds
    @seconds + @minutes * 60
  end

  def self.parse(integer)
    unit = 100
    minutes = integer / 60 / unit
    seconds = integer / unit % 60
    milisec = integer % unit

    new(minutes: minutes, seconds: seconds, miliseconds: milisec)
  end
end
