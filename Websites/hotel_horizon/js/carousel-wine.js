"use strict";
// Select all slides
const slides = document.querySelectorAll(".slide");

// loop through slides and set each slides translateX
slides.forEach((slide, indx) => {
        slide.style.transform = `translateX(${indx * 100}%)`;
});

// select next slide button
const nextSlide = document.querySelector(".btn-next");
const midSlide = document.querySelector(".btn-mid");
const prevSlide = document.querySelector(".btn-prev");
nextSlide.style.setProperty('background',"#6b8e23");

// current slide counter
let curSlide = 0;
// maximum number of slides
let maxSlide = slides.length - 1;

// add event listener and navigation functionality
nextSlide.addEventListener("click", function () {
        curSlide = 0;
        midSlide.style.removeProperty('background');
        prevSlide.style.removeProperty('background');
        nextSlide.style.setProperty('background',"#6b8e23");

        //   move slide by -100%
        slides.forEach((slide, indx) => {
                slide.style.transform = `translateX(${100 * (indx - curSlide)}%)`;
        });
});


// add event listener and navigation functionality
prevSlide.addEventListener("click", function () {
        curSlide = 2;
        nextSlide.style.removeProperty('background');
        midSlide.style.removeProperty('background');
        prevSlide.style.setProperty('background',"#6b8e23");

        //   move slide by 100%
        slides.forEach((slide, indx) => {
                slide.style.transform = `translateX(${100 * (indx - curSlide)}%)`;
        });
});

// add event listener and navigation functionality
midSlide.addEventListener("click", function () {
        curSlide = 1;
        nextSlide.style.removeProperty('background');
        prevSlide.style.removeProperty('background');
        midSlide.style.setProperty('background',"#6b8e23");

        //   move slide by 100%
        slides.forEach((slide, indx) => {
                slide.style.transform = `translateX(${100 * (indx - curSlide)}%)`;
        });
});
