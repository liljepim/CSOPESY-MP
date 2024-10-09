#include <iostream>
#include "ConsoleManager.h"
#include <stdlib.h>

typedef std::string String;

void printMenu() {
	std::cout << "    {__     {__ __      {____     {_______  {________  {__ __  {__      {__\n";
	std::cout << " {__   {__{__    {__  {__    {__  {__    {__{__      {__    {__ {__    {__ \n";
	std::cout << "{__        {__      {__        {__{__    {__{__       {__        {__ {__   \n";
	std::cout << "{__          {__    {__        {__{_______  {______     {__        {__     \n";
	std::cout << "{__             {__ {__        {__{__       {__            {__     {__     \n";
	std::cout << " {__   {__{__    {__  {__     {__ {__       {__      {__    {__    {__     \n";
	std::cout << "   {____    {__ __      {____     {__       {________  {__ __      {__     \n";
	std::cout << "Hello, welcome to the CSOPESY command line!\n";
	std::cout << "Type 'exit' to quit, 'clear' to clear the screen\n";

}

int
main()
{
	ConsoleManager::initialize();

	bool running = true;

	while(running)
	{
		ConsoleManager::getInstance()->process();
		ConsoleManager::getInstance()->drawConsole();

		running = ConsoleManager::getInstance()->isRunning();
	}
	
	return 0;
}