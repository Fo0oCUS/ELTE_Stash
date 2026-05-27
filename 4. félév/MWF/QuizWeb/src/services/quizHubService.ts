import * as signalR from '@microsoft/signalr';

let connection: signalR.HubConnection | null = null;

export const quizHubService = {
    async connect(joinCode: string){
        if(connection) return connection

        connection = new signalR.HubConnectionBuilder().withUrl('/hubs/quiz').withAutomaticReconnect().build();

        await connection.start();
        await connection.invoke("JoinQuizGroup", joinCode);

        return connection;
    },


    onQuestionChanged(callback: () => void){
        connection?.on("QuestionChanged", callback);
    },

    onQuestionClosed(callback: (correctAnswerIndex: number) => void){
        connection?.on('QuestionClosed', callback);
    },

    onQuizEnded(callback: () => void){
        connection?.on("QuizEnded", callback);
    },

    onMessageSent(callback: (message: string) => void){
        connection?.on("MessageSent", callback);
    },

    async disconnect(joinCode: string){
        if(!connection) return;

        await connection.invoke("LeaveQuizGroup", joinCode);
        await connection.stop();
        connection = null;  
    }
}