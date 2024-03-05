# Educative deformable water model

An educative deformable water model for better understanding of surface water dynamics.  
Exploring real-time simulation of dynamic water effects using computer graphics techniques.  

The main challenge addressed was achieving authentic and fluid water simulations within an interactive environment.  
Employing **OpenGL** and specific algorithms, software was developed to realistically depict rain, fluid spreading, and waves.  
Interactive controls were implemented to modify simulation parameters such as wave amplitude and frequency.  

Results demonstrate the achievement of a simulation that offers users an authentic experience of dynamic water effects in a virtual environment.  

## Simulation of water deformation

Simulation of a trochoidal or Gerstner wave 

![gerstner wave mesh](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/260e5a57-311b-4e9e-9b8d-cca80c24b2a0)
![gerstner wave texture](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/8e305e2a-40da-4161-ae51-922c0bc03875)

Simulation of a ripple effect

![circle wave mesh](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/bb28ebe3-ee4f-46a6-a81f-e478e71a2705)
![circle wave texture](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/8406de52-cd4d-4e0c-99dd-6cd34866b660)

## User interaction

Simulating a ripple effect when the user clicks on the water

![mouse click mesh](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/2865f742-fadb-488b-8654-90ae4ad7c08a)
![mouse click texture](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/60e5e933-1d9f-4bc2-9364-2a001813dafa)

## Parameters configuration

Ability to configure parameters of the wave, such as amplitude, frequency, speed and direction

![parameter configuration mesh](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/0a34f583-d066-4059-93cd-ed9ff219a160)
![parameter configuration texture](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/124c186c-1beb-4aa4-ae97-2823f9823868)

## Rain effect

Simulating a rain effect by generating ripples at random points that simulate the effect of rain drops falling on water

![rain mesh](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/23d4938c-b122-491f-892a-ece139182b0d)
![rain texture](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/875fa0dc-6516-424a-bdd0-c243a79f0cd3)

## Spill effect (fluid spreading)

Simulating an effect of fluid spreading on the water

![spill one](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/cc83b08d-d76f-41d9-af8d-91fe0950de96)
![spill many](https://github.com/DaniloBulatovic/Educative-deformable-water-model/assets/93370954/1ffc938a-7ceb-4724-8c19-d395c502376a)

## Controls

| Key | Description |
| :---: | :--- |
| W, A, S, D  | movement of camera up, left, down and right |
| ←, ↑, ↓, → | adjusting camera pitch and yaw |
| T | toggle wave simulation |
| M | toggle between mesh view and filled view (with textured water) |
| R | toggle rain effect |
| B | toggle spill (fluid spreading) effect (visible in filled view) |
| 1, 2, 3 | select ripple size (1 = small, 2 = medium / default, 3 = big) |
| X, C | change wave direction counterclockwise and clockwise |

To configure amplitude, frequency, speed and steepness of the wave,  
first select the parameter that you want to modify by pressing one of the following keys:  

- **A** - amplitude
- **F** - frequency
- **S** - speed
- **P** - steepness  

followed by **+** to increase or **-** to reduce the value.  
Keep pressing or hold **+** or **-** to keep increasing or reducing the value.
