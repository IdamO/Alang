# Alang

![Alt text](https://cloud.githubusercontent.com/assets/13319677/10723591/4b3c813c-7b8a-11e5-9596-d229c8b29273.jpg "Come Alang™ with me :)")

Inspired by fitness tracking bands, Alang is a language tracking tool that provides users who are living in a foreign environment a daily grammar mistake summary.

Unlike typing which can give you immediate feedback, speaking cannot without someone in front of you. With Alang, all you need to do is go about your day speaking the language you are trying to learn. At the end of the day, you will get a summary about all the grammar mistakes that you made during the day.

## Technical details
Alang is composed of two parts: a Pebble app and a web application built using React, Redux, Node, and Socket.io.

Users speak into the Pebble app, written in C, which uses Pebble’s Dictation API for speech-to-text
The Pebble app uses Javascript to send the text to an API and receive corrections. (stretch goal: display these corrections on-device)
The API updates its state in-memory (stretch goal: use Redis or some other persistence) and sends out updates to all connected web clients.

## Next steps
* Display corrections on-device as soon as they are received
* Use a database to store grammar suggestions
* Provide daily / monthly summaries and more helpful tips on mobile application

