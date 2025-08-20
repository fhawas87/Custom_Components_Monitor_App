
<h2><p>Custom Component Monitor application created using OpenGl with GLEW, ImGui with ImPlot. Each chart is beeing updated with retrieved data from few APIs and system files.</p></h2>
<h2><p>Application works only for Linux with Nvidia GPU and Intel CPU at this moment.

<h2>CPU</h2>
<ul>
    <li><p>Whole CPU Utilization            -  /proc/stat</p></li>
    <li><p>Temperature of each CPU Core     -  libsensors (sensors/sensors.h)</p></li>
    <li><p>Frequency of each CPU Core       -  /sys/device/system/cpu/cpu%zu/cpufreq/scaling_cur_freq</p></li>
</ul>

<h2>GPU</h2>
<ul>
    <li><p>GPU Temperature      -  nvml.h</p></li>
    <li><p>GPU Usage            -  nvml.h</p></li>
    <li><p>GPU Frequency        -  nvml.h</p></li>
    <li><p>GPU VRAM Usage       -  nvml.h</p></li>
    <li><p>GPU Power Usage      -  nvml.h</p></li>
</ul>

<h2>RAM</h2>
<ul>
    <li><p>RAM Memory Usage (maybe will play with more parameters)      -  /proc/meminfo</p></li>
</ul>

<h2>FANS</h2>
<ul>
    <li><p>Available Fans Speed in RPM (only two fans are visible in my case. I believe it's caused by chips problems)      -  libsensors (sensors/sensors.h)</p></li>
</ul>



<p allign="center">
    <img src="Screenshots/full_app_light_grey_theme.png" alt="LIGHT GREY THEME APP VIS" width="1199">
    <img src="Screenshots/1.png" alt="1" width="270">
    <img src="Screenshots/2.png" alt="2" width="270">
    <img src="Screenshots/3.png" alt="3" width="270">
    <img src="Screenshots/first_full_app.png" alt="aa" width="1199">
</p>

<h2><p>Also console visualization below (created firstly for debugging).</p></h2>

<p allign="left">
    <img src="Screenshots/Console_Demo_Screenshot.png" alt="DEMO_1" width="350">
    <img src="Screenshots/Demo_Console_1.png" alt="DEMO_2" width="400">

