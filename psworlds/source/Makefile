# makefile for possible worlds - linux
# by The Brain & Eikzilla
#
# makefile by eik
# thx to Mika Boström

SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)
FMOD_FLAGS := -I../../fmod/inc -L../../fmod/lib -L../../fmod
CFLAGS := -DPLATFORM_LINUX  -c -o


all: alienlight.o camera.o cockpit.o compass.o bitmap.o briefing.o building.o computer.o direct.o displaylistlist.o fighter.o fonts.o headlight.o homing.o intro.o matrices.o menu.o missile.o mission.o model.o object.o objectlist.o other.o outro.o particle.o player.o psworlds.o startscreen.o sound.o texturelist.o tgaloader.o torpedo.o trail.o turret.o utility.o waypoint.o world.o
	gcc -o ../psworlds-linux alienlight.o camera.o cockpit.o compass.o bitmap.o briefing.o building.o computer.o direct.o displaylistlist.o fighter.o fonts.o intro.o headlight.o homing.o matrices.o menu.o mission.o missile.o model.o outro.o object.o objectlist.o other.o particle.o player.o psworlds.o startscreen.o sound.o texturelist.o tgaloader.o torpedo.o trail.o turret.o waypoint.o utility.o world.o $(FMOD_FLAGS) -lfmod -L/usr/X11R6/lib -lX11 -lXext -lglut -lGL -lGLU -lfmod-3.33 `sdl-config --cflags --libs`


alienlight.o: alienlight.cpp
	gcc $(CFLAGS) alienlight.o alienlight.cpp

camera.o: camera.cpp
	gcc $(CFLAGS) camera.o camera.cpp

cockpit.o: cockpit.cpp
	gcc $(CFLAGS) cockpit.o cockpit.cpp

compass.o: compass.cpp
	gcc $(CFLAGS) compass.o compass.cpp

bitmap.o: bitmap.cpp
	gcc $(CFLAGS) bitmap.o bitmap.cpp

briefing.o: briefing.cpp
	gcc $(CFLAGS) briefing.o briefing.cpp

building.o: building.cpp
	gcc $(CFLAGS) building.o building.cpp

computer.o: computer.cpp
	gcc $(CFLAGS) computer.o computer.cpp

direct.o: direct.cpp
	gcc $(CFLAGS) direct.o direct.cpp

displaylistlist.o: displaylistlist.cpp
	gcc $(CFLAGS) displaylistlist.o displaylistlist.cpp

fighter.o: fighter.cpp
	gcc $(CFLAGS) fighter.o fighter.cpp

fonts.o: fonts.cpp
	gcc $(CFLAGS) fonts.o fonts.cpp

homing.o: homing.cpp
	gcc $(CFLAGS) homing.o homing.cpp

headlight.o: headlight.cpp
	gcc $(CFLAGS) headlight.o headlight.cpp

intro.o: intro.cpp
	gcc $(CFLAGS) intro.o intro.cpp

matrices.o: matrices.cpp
	gcc $(CFLAGS) matrices.o matrices.cpp

menu.o: menu.cpp
	gcc $(CFLAGS) menu.o menu.cpp

missile.o: missile.cpp
	gcc $(CFLAGS) missile.o missile.cpp

mission.o: mission.cpp
	gcc $(CFLAGS) mission.o mission.cpp

model.o: model.cpp
	gcc $(CFLAGS) model.o model.cpp

outro.o: outro.cpp
	gcc $(CFLAGS) outro.o outro.cpp

object.o: object.cpp
	gcc $(CFLAGS) object.o object.cpp

objectlist.o: objectlist.cpp
	gcc $(CFLAGS) objectlist.o objectlist.cpp

other.o: other.cpp
	gcc $(CFLAGS) other.o other.cpp

particle.o: particle.cpp
	gcc $(CFLAGS) particle.o particle.cpp

player.o: player.cpp
	gcc $(CFLAGS) player.o player.cpp

psworlds.o: psworlds.cpp
	gcc $(CFLAGS) psworlds.o psworlds.cpp

startscreen.o: startscreen.cpp
	gcc $(CFLAGS) startscreen.o startscreen.cpp

sound.o: sound.cpp
	gcc $(FMOD_FLAGS) $(CFLAGS) sound.o sound.cpp

texturelist.o: texturelist.cpp
	gcc $(CFLAGS) texturelist.o texturelist.cpp

tgaloader.o: tgaloader.cpp
	gcc $(CFLAGS) tgaloader.o tgaloader.cpp

trail.o: trail.cpp
	gcc $(CFLAGS) trail.o trail.cpp

torpedo.o: torpedo.cpp
	gcc $(CFLAGS) torpedo.o torpedo.cpp

turret.o: turret.cpp
	gcc $(CFLAGS) turret.o turret.cpp

utility.o: utility.cpp
	gcc $(CFLAGS) utility.o utility.cpp

waypoint.o: waypoint.cpp
	gcc $(CFLAGS) waypoint.o waypoint.cpp

world.o: world.cpp
	gcc $(CFLAGS) world.o world.cpp


clean:
	rm -f alienlight.o
	rm -f camera.o
	rm -f cockpit.o
	rm -f bitmap.o
	rm -f briefing.o
	rm -f building.o
	rm -f computer.o
	rm -f compass.o
	rm -f direct.o
	rm -f displaylistlist.o
	rm -f fighter.o
	rm -f fonts.o
	rm -f headlight.o
	rm -f homing.o
	rm -f intro.o
	rm -f matrices.o
	rm -f menu.o
	rm -f missile.o
	rm -f mission.o
	rm -f model.o
	rm -f object.o
	rm -f objectlist.o
	rm -f other.o
	rm -f outro.o
	rm -f particle.o
	rm -f player.o
	rm -f psworlds.o
	rm -f startscreen.o
	rm -f sound.o
	rm -f texturelist.o
	rm -f tgaloader.o
	rm -f torpedo.o
	rm -f trail.o
	rm -f turret.o
	rm -f utility.o
	rm -f waypoint.o
	rm -f world.o
	










