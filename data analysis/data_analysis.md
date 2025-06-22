# Turtle Data Logger Analysis

This report shows a simple analysis of turtle movement data. The data was collected using a small device attached to a turtle. We used Python and Google Colab to do the analysis.


## 📓 Google Colab Notebook

You can see the full analysis here:  
[Open Colab Notebook](https://colab.research.google.com/drive/19DCzMrziEgvls1B00Xk3MgblnVQLi2vJ?usp=sharing)


## 📁 Dataset

The dataset used in this project is saved in Google Drive:  
[Download or View Dataset](https://drive.google.com/drive/folders/1V7QemAyoJ-9IaNh3CJIsgkKc8PWI8eDD?usp=sharing)


## 🕒 Data Collection Period

- **Start Date & Time:** May 2, 2025 at 16:00:01 (approximately, ±1 hour)  
- **End Date & Time:** May 10, 2025 at 03:19:44 (approximately, ±1 hour)  

This means the data was recorded over a period of around 8 days.


## 📈 Acceleration Time Series Visualization

The chart below shows the turtle’s movement using acceleration magnitude. The x-axis represents the sample index (each sample is one data point recorded over time). Spikes in the graph show active movement, while flat areas mean less or no movement.

![Turtle Acceleration Chart](https://github.com/user-attachments/assets/9cdec641-cf7f-4678-9c5c-114b28ae9a90)


## 📊 Statistical Overview of Turtle Movement

To understand the turtle's activity, we calculated some basic statistics from the acceleration magnitude data:

- **Mean Acceleration:** 10.01 m/s²  
- **Standard Deviation:** 0.13 m/s²  
- **Movement Threshold:** 10.14 m/s² (Mean + Standard Deviation)

The **mean** shows the average acceleration, which is close to gravity (1g ≈ 9.81 m/s²), meaning the turtle was often still.  
The **standard deviation** shows how much the values vary. We used the threshold (mean + std) to detect active movement — when acceleration is significantly above normal.


## 🔍 Percentage of Active Time

To find how often the turtle was active, we marked data points where the acceleration was higher than the movement threshold (10.14 m/s²).

- **Active Time:** 6.24%

This means the turtle showed noticeable movement in about **6.24% of the total recorded time**. The rest of the time, it was likely resting or moving very slowly.


## 🕐 Daily Activity Cycle: Turtle Movement by Hour

This chart shows the turtle's movement pattern across different hours of the day. The data is grouped by hour to see when the turtle was most active.

![Hourly Turtle Activity](https://github.com/user-attachments/assets/8d240d50-2a06-4909-92fe-a9f45e291d45)


## 🌡️ Heatmap of Turtle Activity by Hour and Date

This heatmap shows how active the turtle was at different hours on different days.  
Each cell represents the **percentage of active time** during a specific hour on a specific date.

- **Darker colors** = more activity
- **Lighter colors** = less or no activity

![Turtle Activity Heatmap](https://github.com/user-attachments/assets/8bc5e0a4-9792-4091-a996-ad25efd3e04e)


