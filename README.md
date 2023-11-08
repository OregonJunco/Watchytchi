# Watchytchi
Watchytchi is a Tamagotchi-like virtual pet for [Watchy](https://watchy.sqfmi.com/), an open-source e-ink smart watch.

I love virtual pets and always wanted to make my own. I'm primarily a Unity developer with little microelectronics experience, so I chose to develop for Watchy because it come preassembled with buttons, a screen, and a real-time-clock. With that said, this is my first experience working with Arduino and ESP32 and I'm mostly hacking in my spare time. Messy code ahead! 

The code for the Watchy project can be found in the "Watchytchiuno" folder. The project compiles and works on device but it is a rough first draft, with some messy code and missing freatures. I hope to flesh it out into a more complete experience! The "Watchytchi" folder contains an an all-software proof of concept I made in [Processing](https://processing.org/) before I had access to the hardware.

 <div>
 <img src="./Github%20Assets/Watchytchi_Idle_Gb.jpg" alt="Two Watchys with the pet" width="205"/> 
 <img src="./Github%20Assets/Watchytchi_HaunchesWithPoop_ClearCase.jpg" alt="The pet in idle pose" width="190"/> 
 <img src="./Github%20Assets/Watchytchi_TiredNight.jpg" alt="A tired pet at night, lit by a lightbulb" width="200"/> 
 </div>

## Instructions:
### Navigation:
 - **The upper-left button is "Cursor,"** which moves your menu selection. **The upper right button is "Confirm,"** which you use to select options. **The lower right button is "Cancel,"** which backs out of menus and resets your cursor position. **The lower left button is "Settings,"** and opens up the various Watchy firmware options, such as time set, vibration test, etc.
 - Pressing the "Cursor" button cycles your selection between the care icons on the top and bottom. A selected option is drawn in all black, while unselected options are grayed out (dithered)
 - The device vibrates when you advance your cursor or select an option
### <img src="./Watchytchi/data/MenuIcon_Status_Active.png" alt="Magnifying Glass Icon" width="16"/> Inspect
Selecting the magnifying glass will open up the "inspect" UI, showing your pet's hunger (left) and age (right). 
 - For hunger, 0 is the hungriest possible, and 9 is full. 
 - The age is the number of day's since your pet's "Birth." It goes up to a maximum of 9. Age currently has no effect on your pet
 - Press the "Cancel" button to close this display.
### <img src="./Watchytchi/data/MenuIcon_Feed_Active.png" alt="Fridge Icon" width="16"/> Hunger and Feeding
 - Your pet becomes hungrier over time
 - Your pet's idle sprite will change depending on their hunger. If they are below 4 hunger, their trunk droops and they have a frazzled expression. If they are below 1 hunger, they will lie on the ground and look unhappy.
 - To notify the owner, your pet's vibration motor will buzz once when it reaches below 4 hunger, and vibrate repeatedly on an interval when it is below 1 hunger.
 - To feed your pet, move your cursor to the "refrigerator" icon and press "Confirm." A food item will spawn, and your pet will slowly chomp at it. Input is blocked during this animation. Feeding makes your pet completely full, so there is no need to feed more than once in a row.
### <img src="./Watchytchi/data/MenuIcon_Alert_Active_HasNotification.png" alt="Notification Icon" width="16"/> Special Events
 - Special interactive events will sometimes occur for limited periods during the day. 
 - If there is an active event, the "Speech balloon" icon will have an exclamation point in it. Moving your cursor to this icon and hitting "select" will active the event
 - If there is no available event, the speech balloon icon will be grayed out and not be selectable
 - Activating an event will cause your pet to appear large on the screen for a short "close-up" animation
 - In the future, there will be other responses to activating events
### <img src="./Watchytchi/data/MenuIcon_Clean_Active.png" alt="Sponge Icon" width="16"/>  Poop and Cleanup
 - Every 4 hours or so, your pet will poop on the ground
 - To clean up poop, move your cursor to the "sponge" icon and press "Confirm." The poop will disappear.
### <img src="./Watchytchi/data/MenuIcon_Lights_Active.png" alt="Lightbulb Icon" width="16"/> Time of day
 - Your pet's world responds to the real-world time of day
 - During the morning and afternoon, a sun slowly moves across the screen. This is replaced by a moon during the evening and night. Clouds drift through the sky over time
 - In the early evening, a dithered vignette will appear at the edges of the screen
 - At night, a dark vignette will cover much of the screen. A lightbulb will hang down and act as a "light source." Your pet will lay down and look sleepy. 
 - To put your pet to bed, select the "lightbulb" care icon. This turns out the lights and inverts the screen colors. Your pet will sleep peacefully.
 - The lights automatically turn back on in the daytime.
 - Thresholds for day and night are hardcoded to certain hours, and do not respond to local sunrise / sunset times (i.e. longitude and time of year)
 - While this software is not a true watch face, there is a small time display in the lower right for debugging purposes
### Age
 - To visualize your pet's age, there is a small potted plant on the right hand side. Every day the plant will grow a little, until it blooms into a flower. There are currently 6 day's worth of growth stages for the flower
### Idle animations
 - During the day, if your pet is not hungry, they may display idle animations
 - Every 3 minutes they perform a short animation
 - Depending on the time of day, their default idle pose may be replaced by special alternative poses
# Limitations and known bugs
This is very much a work in progress project and has several large issues. Due to my inexperience with microelectronics and C++, some of these may take a long time to fix
 - ~~All data is lost when the battery runs out. I don't know quite how to make data fully persistent.~~ Should be fixed now that I've implemented AndroidNVS, although I haven't tested full power cycling
 - The menu is not very responsive and it takes a long time to scroll through options. This is caused by two main factors: first, the watch exits and returns to deel sleep after every button presss. Second, the software is redundantly redrawing the whole face on button presses instead of just the relevant icons
 - ~~Battery life is very poor, perhaps due to updating the screen too frequently or to overusing the vibration motor~~ We now seem to be at around 7 days!
 - Most of the menu buttons don't do anything due to many features not being implemented. They appear as question marks. A "Book" icon appears in the lower right, but has no function.
 - Certain time passage calculations are being done in a hacky way that does not handle the wrapping around from 0 to 59 minutes, 31st day of the month to 1st day of the month, etc.
 - "Watchytchi" is a mouthful, I know. And "Tamawatchy" was right there. I do not apologize.
 
# Planned features
Watchytchi is a functional prototype, but I hope to flesh it out into a complete experience. I still need to do some game design, but here are some features on my mind:
 - ✅ ~~Persistent save data between flashes and power cycles~~ Implemented!
 - Optimization to increase battery life
 - Partial-redraw implementation on pressing the cursor advance button to greatly increase menu navigation speed
 - A low battery indicator
 - Support for multiple creature species and "evolutions," similar to a traditional Tamagotchi
 - A "Babysitting" function you can activate if you know you won't be able to take care of your pet on a given day
 - Consequences for poor care and neglect (perhaps lowering lifespan, and death in extreme cases)
 - Playful activities to do with your creature such as stroking, reading a book, and/or playing with toys!
 - ✅ ~~Poop and poop cleanup functions~~ Implemented!
 - More idle animations for variety
 - A finite "lifespan," creating a proper end to the experience

# Non-planned features
There are a few features which are common to many watch faces / virtual pets, but which I don't plan to implement:
 - I plan for this to be an unconnected device with no wi-fi or bluetooth functionality, for a few reasons: project scope, lack of personal experience with those technologies, battery optimization, and a general desire for this to be a self-contained piece of software that won't break if APIs change
 - I don't care very much for the "games" included in most tamagotchis, which are usually so simple as to feel like chores you try to get through as quickly as possible to raise your happy meter. I also have no intention of doing digimon-style battling
 - Similarly, I don't plan to implement currency or an item inventory
 - The "weight" stat on tamagotchis never appealed to me much

# Acknowledgements
Thank you to Devign999 for providing your excellent [dktime](https://github.com/dezign999/dkTime) watch face as a free online resource, which served as a fantastic example of animations. In addition, thank you to everyone on the Watchy discord server who has answered my questions!

![The Watchy Creature](./Github%20Assets/WatchytchiEatingGif.gif?raw=true "Title")
