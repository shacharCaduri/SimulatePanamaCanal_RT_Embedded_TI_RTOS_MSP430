# **SimulatePanamaCanal_RT_Embedded_TI_RTOS_MSP430**
### **IMPORTANT BEFORE READING!!!**
This Project has been done using the lecturer guidance and all photos here are the copyrights of this lecturer only and thus must not be used or copied in any kind of way anywere. Those photos represented here are only to make the reader here in this README file understand part of what has been done in the project and not by any means not using it or copy it in any other way except seeing it here.

### **Description**
Given a description of panama canal and how to simulate it, in this project in college course "Real Time Embedded Systems" we were asked to make a simulation of panama canal using TI RTOS Kernel and on MSP430 micro controller card on the Code Composer Studio IDE while following the rules of multithreaded programming and Real Time systems with priority  based scheduler and thus simulating the panama canal as described in the final project specifications. The Conceptual Abstraction for simulating Panama Cannal that this project followed is as shown here:

![Conceptual Abstraction - Panama Canal](https://user-images.githubusercontent.com/73823590/141675470-ef6ab7f5-63d9-493b-be9b-bc8beeb3ba31.PNG)

This has been done with using OS and RTOS system concepts like putting a clock to make the RTOS simulate time sharing system which are not assuming anything about the when threads will gain\lost CPU,
or using locks to synchronizing objects that wants to get access to shared location using all kinds of semaphores: Resource Control Semaphores, Mutexes, and Scheduling Constraint Semaphores, defend from deadlocks and synchronization problems or timing parts of the system in diffrent ways like changing priorties instead of using semaphores, and Etc. see source code for more.

### **What the application does?**
Simulating Panama Canal using multithreaded programming and RTOS (real time operating system) principles.

### **Why I used this technologies?**
I used TI-RTOS kernel with the IDE Code Composer because it was a college course project which was required in the course's syllabus.

### **Some challenges**
One of the challenges I faced is one of the critical challenges in Real Time Embedded projects is memory management, cause all things need to be done in a manner of saving memory, timing for buisness or safety crictical systems and this is because:

   a. those systems usually have small / limited memory capicity proportion to the system needs. 

   b. those systems usually have one main goal which it needs to accomplish and using the memory in a smart and efficient way.
  
### **How to install**
To install this project (INSTALLATION through Code Composer Studio) :

   a. Download the project as an archived zip file (you can do it by clicking the green button 'Code' then 'Download ZIP').
   
   b. Open Code Composer Studio (CCStudio) then click on the TAB project:
   
   ![click on project](https://user-images.githubusercontent.com/73823590/141674064-c98be97d-604b-45f5-a2e8-e2ace3063658.PNG)
   
   c. Click on import CCS Project:
   
   ![import ccs project](https://user-images.githubusercontent.com/73823590/141674104-4a43964d-3fb7-48ce-ae0f-563027f55f0c.png)
   
   d. In the opened window that opened click on browse, and browse the archived zip version of this project you dowloaded earlier:
   
   ![click browse search your project winrar zip archive](https://user-images.githubusercontent.com/73823590/141674164-06964eb7-0c44-4672-a9e4-69ca8489ed6f.png)

e. After that you will be able to click on the finish button, press finish and your project is installed on CCStudio.

![click finish](https://user-images.githubusercontent.com/73823590/141674207-faaf6b10-4c6c-49fe-bbde-d941b0589227.PNG)

Note: somtimes you'll see the discover project (such as here in my case) with another name because I have this same exact content of a project with diffrent name.

For any kind of issues or suggestions in the context of this installation you can contact me.

### **How to use?**
This project purpose for simulation only with the MSP430F5529 Micro controller card, and you use it by running it on this micro controller card that has been specified earlier here and in the description, I suggest anyone who want to know how to run will search for a guide how to run a project on CCStudio even tough it is done by simply search the button build\rebuild and then click debug and let it run, Further more it is important to mention that because this project run on a micro controller card it also has led blink service specification in simple words it is indicators on the card that it is running on and the indicators are the leds that are turning on according to this specification:


![led blink specification](https://user-images.githubusercontent.com/73823590/141675111-2cdbf1ad-165a-49cc-bec4-4d62c768557b.PNG)

Per Map Point:

![led blink specification per map point](https://user-images.githubusercontent.com/73823590/141675114-8925b84a-0620-4288-973d-0337ba6e4eff.PNG)

The LEDs on the conroller should blink as described here in the photos, and the user can debug and see the behavior of the the threads in the system and thus see it is actually simulating the panama cannal as show in the conceptual abstraction in the description earlier in this document.
