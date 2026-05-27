import type { JoinRequestDto, GetQuizByJoinCodeRequestDto, QuizRequestDto, QuizMessageRequestDto } from "../Models/requestDtos";
import type { QuizResponseDto } from "../Models/responseDtos";
import { api } from "./api";

export const quizService = {

    async addQuiz(data: QuizRequestDto): Promise<QuizResponseDto>{
        const response = await api.post("/quizzes", data);
        return response.data;
    },

    async updateQuiz(id: number, data: QuizRequestDto): Promise<QuizResponseDto>{
        const response = await api.post("/quizzes/"+id+"/update", data);
        return response.data;
    },

    async getUserQuizzes(): Promise<QuizResponseDto[]>{
        const response = await api.get("/quizzes/mine");
        return response.data;
    },

    async getQuiz(id: number): Promise<QuizResponseDto>{
        const response = await api.get("/quizzes/" + id);
        return response.data;
    },

    async publishQuiz(id: number): Promise<QuizResponseDto>{
        const response = await api.get("/quizzes/publish/" + id);
        return response.data;
    },

    async joinQuiz(data: JoinRequestDto): Promise<boolean>{
        const response = await api.post("/quizzes/join", data);
        return response.data;
    },

    async getQuizByJoinCode(data: GetQuizByJoinCodeRequestDto): Promise<QuizResponseDto>{
        const response = await api.post("/quizzes/code", data);
        return response.data;
    },

    async nextQuestion(id: number){
        const response = await api.get("/quizzes/"+id+"/next");
        return response.data;   
    },
    
    async endQuiz(id: number){
        const response = await api.get("/quizzes/"+id+"/end");
        return response.data;   
    },

    async closeCurrentQuestion(id: number){
        const response = await api.get("/quizzes/"+id+"/question/end");
        return response.data;
    },

    async sendMessage(data: QuizMessageRequestDto){
        const response = await api.post("/quizzes/message", data);
        return response.data;
    }

}