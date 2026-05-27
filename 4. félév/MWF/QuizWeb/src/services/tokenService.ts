const TOKEN_KEY = "authToken"
const REFRESH_TOKEN_KEY = "refreshToken"

export const tokenService = {
    setToken(token: string){
        localStorage.setItem(TOKEN_KEY, token);
    },

    getToken(): string | null{
        return localStorage.getItem(TOKEN_KEY);
    },

    removeToken(){
        localStorage.removeItem(TOKEN_KEY);
    },
    
    setRefreshToken(refreshToken: string){
        localStorage.setItem(REFRESH_TOKEN_KEY, refreshToken);
    },

    getRefreshToken(): string | null{
        return localStorage.getItem(REFRESH_TOKEN_KEY);
    },

    removeRefreshToken(){
        localStorage.removeItem(REFRESH_TOKEN_KEY);
    }
}