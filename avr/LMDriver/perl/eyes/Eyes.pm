{ package Eyes;

use strict;
use FileHandle;
use Data::Dumper;
  
use constant ON => 1;
use constant OFF => 0;
  
sub eyeSendChar
{
  my ($self, $charString ) = @_;
  for my $ch (split ('', $charString))
    {
      $self->{fifo}->print($self->{charAccessor} . $ch);
    }
}

sub eyeCustomColumn
{
  my ($self, $mask, $data) = @_;
  my $sendString;
  for my $index (0..4)
    {
      if ($$mask[$index] == 1)
	{
	  $sendString .= $self->{customColumn}[$index] . $$data[$index];

	}
    }
  $self->{fifo}->print($sendString . "\n");
}

sub pupilCenter
{
  my ($self, @series) = @_;
  if (@series)
    {
      for my $undoRoutine (reverse @series)
	{
	  &$undoRoutine($self);
	}
    }
  else
    {
      $self->{fifo}->print('t' . $self->{pixelAccessor} . "42\n");
    }
}

sub evilEye
{
  my ($self) = @_;
    
  my @eyeShape = ('0f', '1e', '3c', '18', '00'); 
  @eyeShape = reverse (@eyeShape) if  ($self->{enum} == 1);
  $self->eyeCustomColumn([1,1,1,1,1], \@eyeShape);
}

sub rageSeries
  {
    my ($self) = @_;
    my @series =
      (
        sub { my $self = shift; $self->{fifo}->print('i' . $self->{pixelAccessor} . "\n") },
       sub { my $self = shift; $self->{fifo}->print('I' . $self->{pixelAccessor} . "\n") },
      );
    return @series;
  }

sub blinkSeries
{
  my ($self) = @_;
  my @series =
    (
     sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "42\n") },
     sub { my $ii = 0; while ($ii < 500000) { $ii++}},
     sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "42\n") },
#     sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "42\n") },
#     sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "42\n") },

#     sub { my $self = shift; $self->eyeCustomColumn([1,1,1,1,1], ['22','41','49','41','22']) },
#     sub { my $self = shift; $self->eyeCustomColumn([1,1,1,1,1], ['14','22','2a','22','14']) },
#     sub { my $self = shift; $self->eyeCustomColumn([1,1,1,1,1], ['08','14','1c','14','08']) },
#     sub { my $self = shift; $self->eyeCustomColumn([0,1,1,1,0], ['08','08','08','08','08']) },
#     sub { my $self = shift; $self->eyeCustomColumn([0,1,1,1,0], ['08','14','1c','14','08']) },
#     sub { my $self = shift; $self->eyeCustomColumn([1,1,1,1,1], ['14','22','2a','22','14']) },
#     sub { my $self = shift; $self->eyeCustomColumn([1,1,1,1,1], ['22','41','49','41','22']) },
    );
  return @series;
}

sub lookLeftSeries
  {
    my ($self) = @_;
    my @series = 
      (
       sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "42\n") },
       sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "42\n") },
       sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "43\n") },
       sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "43\n") },
      );
    return @series;
  }

sub lookRightSeries
  {
    my ($self) = @_;
    my @series = 
      (
       sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "42\n") },
       sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "42\n") },
       sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "41\n") },
       sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "41\n") },
      );
    return @series;
  }

sub lookUpSeries
  {
    my ($self) = @_;
    my @series = 
      (
       sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "42\n") },
       sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "42\n") },
       sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "32\n") },
       sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "32\n") },
      );
    return @series;
  }

sub lookDownSeries
  {
    my ($self) = @_;
    my @series = 
      (
       sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "42\n") },
       sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "42\n") },
       sub { my $self = shift; $self->{fifo}->print('T' . $self->{pixelAccessor} . "52\n") },
       sub { my $self = shift; $self->{fifo}->print('t' . $self->{pixelAccessor} . "52\n") },
      );
    return @series;
  }

sub new
{
  my $class = shift;
  my $accessor = shift;
  
  my $self =
    {
     enum => $accessor,
     paletteAccessor => ($accessor == 0 ? 'l' : 'L'),
     charAccessor => ($accessor == 0 ? 'a' : 'A'),
     pixelAccessor => ($accessor == 0 ? '0' : '1'),
     customColumn => ($accessor == 0 ? [0..4] : [5..9]),
     fifo => new FileHandle '>/tmp/lmdriver.fifo'
    };
  
  unless (defined $self->{fifo})
    {
      die "Error opening fifo";
    }
  bless $self, $class;
  $self->{fifo}->autoflush();
  $self->{fifo}->print('c' . $accessor);
  $self->baseExpression();
 
  return $self;
}

sub frown
{
  my $self = shift;
  if (0 == $self->{enum})
    {
      $self->eyeCustomColumn([1,1,1,1,1], ['00','01','12','04','00']);
    }
else
  {
      $self->eyeCustomColumn([1,1,1,1,1], ['00','04','12','01','00']);
  }

} 
sub baseExpression
  {
    my $self = shift;
    $self->eyeCustomColumn([1,1,1,1,1], ['04','02','12','02','04']);
#    $self->pupilCenter();
#    $self->{pupil} = [\&pupilCenter];
#    &{$self->{pupil}[0]}($self, ON);
#    $self->pupilCenter();
  }

sub setPupil
  {
    my ($self) = @_;
    &{$self->{pupil}[0]}($self, ON);
  }
sub blink
  {
    my $self = shift;
  }
sub color
  {
    my ($self, $color) = @_;
    $self->{fifo}->print(sprintf("%s%02X\n", $self->{paletteAccessor}, $color));
    
  }

sub invert
  {
    my ($self, $state) = @_;
    $self->{fifo}->print(($state == 1 ? 'i' : 'I') . $self->{pixelAccessor} . "\n");
  }
sub blank
  {
    my ($self, $state) = @_;
    $self->{fifo}->print(($state == 1 ? 'b' : 'B') . $self->{pixelAccessor} . "\n");
  }
return 1;
}

