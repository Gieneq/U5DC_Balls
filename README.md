# STMU5 Balls simulation

Some collision check and graphics example with round display.

STM32U5A9NJH6Q [STM32U5A9J-DK](https://www.st.com/en/evaluation-tools/stm32u5a9j-dk.html)

Here you can find:
- MIPI DSI configuration,
- DMA2D usage to fill screen,
- DMA2D usage to copy data,
- double framebuffer with LTDC and GFXMMU for memory usage reduction,
- VSYNC with LTDC line event,

At 30 FPS 10% CPU time - not bad.

[Working example](img/round.png)