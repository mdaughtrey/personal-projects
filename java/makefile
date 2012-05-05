target=HelloWorldSwing

run: $(target).class
	java $(target)


$(target).class: $(target).java
	javac $(target).java


%.class: %.java
	javac $<
