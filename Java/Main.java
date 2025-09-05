import java.util.Scanner;

class Car {
    private Runnable promptPrinter;
    private int year;
    String model;
    int make;
    boolean isOn;

    int fuel = 100;

    // Constructor with year validation
    Car(int yearMade, int carMake, String carModel, Runnable promptPrinter) {
        this.model = carModel;
        this.make = carMake;
        this.promptPrinter = promptPrinter;
        if (yearMade > 1886) {
            this.year = yearMade;
        } else {
            System.out.println("Invalid year! Cars were invented in 1886 or later.");
            this.year = 1886;
        }
        startFuelThread();
    }

    //create necessiary variables for fuel
    private Thread fuelThread;
    private volatile boolean running = false;

    void setPromptPrinter(Runnable promptPrinter){
        this.promptPrinter = promptPrinter;
    }

    private void startFuelThread(){
        fuelThread = new Thread(() -> {
            while(true){
                if(running && fuel > 0){
                    try{
                        Thread.sleep(1000);
                    }catch(InterruptedException e){
                        break;
                    }

                    fuel--;
                    if (promptPrinter != null) promptPrinter.run();
                    if(fuel == 0){
                        System.out.println("Out of fuel! Engine stopping...");
                        stopEngine();
                    }
                }else{
                    try{
                        Thread.sleep(500);
                    } catch(InterruptedException e){
                        break;
                    }
                }
            }
        });
        fuelThread.setDaemon(true);
        fuelThread.start();
    }


    void startEngine() {
        if(fuel == 0){
            System.out.println("Can't start engine, out of fuel!!");
            running = false;
            return;
        }
        System.out.println("Engine sputters for a moment but then spins to life!");
        this.isOn = true;
        running = true;
    }

    void stopEngine() {
        System.out.println("The engine has turned off.");
        this.isOn = false;
        running = false;
    }

    // New method to print car details
    void printCarDetails() {
        System.out.println("Car details: ");
        System.out.println("Make: " + make);
        System.out.println("Model: " + model);
        System.out.println("Year: " + year);
    }
}

public class Main {
    public static void main(String[] args) {
        //create Scanner
        Scanner scanner = new Scanner(System.in);
        
       Car chevvy = new Car(2025, 1, "Chevrolet Corvette", null);

        Runnable promptPrinter = () -> 
        System.out.print("Enter Action ('list' to list all actions): " + chevvy.fuel + "% fuel remaining\n> ");

        chevvy.setPromptPrinter(promptPrinter);

        String[] onPhrases = {
            "turn on",
            "engine on",
            "start",
            "car on",
            "ignition on",
            "on"
        };
        String[] offPhrases = {
            "turn off",
            "engine off",
            "stop",
            "car off",
            "ignition off",
            "off"
        };
        String[] userInteractions = {
            "look for oil",
            "list",
        };

        
        while(true){
            System.out.print("Enter action (list to see all actions): ");
            String action = scanner.nextLine().toLowerCase();
            
            if(action.equals("exit")){
                System.out.println("program stopping...");
                scanner.close();
                break;
            }

            //20% chance of random event happening
            if(Math.random() < 0.2 && chevvy.isOn){
                //random numbers 0-4
                int randEvent = (int)(Math.random() * 5);
                switch(randEvent){
                    case 0:
                        System.out.println("fuel cap left open! Fuel decreased by 10 points");
                        chevvy.fuel = chevvy.fuel - 10;
                        if(chevvy.fuel < 0) chevvy.fuel = 0;
                        break;
                    case 1:
                        System.out.println("Engine busted! Engine turned off and unavalible for 5 seconds");
                        chevvy.stopEngine();
                        try{
                            Thread.sleep(5000);
                        }catch(InterruptedException e){
                            e.printStackTrace();
                        }
                        System.out.println("Engine fixed! You can now turn the engine back on");
                        break;
                    case 2:
                        System.out.println("You found a gas station! Fuel increased by 20 points");
                        chevvy.fuel = chevvy.fuel + 20;
                        if(chevvy.fuel > 100) chevvy.fuel = 100;
                        break;
                    case 3:
                        System.out.println("Grace! You won't lose fuel for the next 10 seconds");
                        try{
                            Thread.sleep(10000);
                        }catch(InterruptedException e){
                            e.printStackTrace();
                        }
                        System.out.println("Grace period over! You will now lose fuel again");
                        break;
                    case 4:
                        System.out.println("You got a ticket for reckless driving! Engine turned off for 10 seconds");
                        chevvy.stopEngine();
                        try{
                            Thread.sleep(10000);
                        }catch(InterruptedException e){
                            e.printStackTrace();
                        }
                        chevvy.startEngine();
                        break;
                    default:
                        break;
                }
            }

            for(String phrases : userInteractions){
                if(action.contains(phrases)){
                    if(phrases.equals("look for oil") && chevvy.isOn){
                        //random chance of finding oil, 4/10 chance of finding oik
                        if(Math.random() < 0.4){
                            System.out.println("You found oil! Fuel increased by 10 points");
                            chevvy.fuel = chevvy.fuel + 10;
                            if(chevvy.fuel > 100) chevvy.fuel = 100;
                            break;
                        }else {
                            System.out.println("You didn't find any oil.");
                            break;
                        }
                    } else if (phrases.equals("look for oil") && !chevvy.isOn){
                        System.out.println("You need to have the engine on to look for oil!");
                        break;
                    }
                }else if (action.equals("list")){
                        //list userInteraction
                        System.out.println("Available actions:");
                        for(String phrase : userInteractions){
                            System.out.println("- " + phrase);
                        }
                }
            }

            if(!chevvy.isOn){ //if its off
                for(String phrase : offPhrases){
                    if(action.contains(phrase)){
                        System.out.println("Engine is already off!");
                        break;
                    }
                }
                for(String phrase : onPhrases){
                    if(action.contains(phrase)){
                        chevvy.startEngine();
                        break;
                    }
                }

            }else{ //if its on
                for(String phrase : offPhrases){
                    if(action.contains(phrase)){
                        chevvy.stopEngine();
                        break;
                    }
                }
                for(String phrase : onPhrases){
                    if(action.contains(phrase)){
                        System.out.println("Engine if already on!");
                        break;
                    }
                }
            }
        }
    }
}
