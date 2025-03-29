import React, { useState, useEffect, useCallback } from "react";
import axios from "axios";
import { Container, Typography, TextField, Button, Alert, Box } from "@mui/material";
import { useNavigate } from "react-router-dom";


function Withdraw() {
  const [balance, setBalance] = useState(0);
  const [amount, setAmount] = useState("");
  const [message, setMessage] = useState("");
  const navigate = useNavigate();
  const accountId = localStorage.getItem("accountId");


  const fetchBalance = useCallback(async () => {
    try {
      const res = await axios.get(`/balance/${accountId}`);
      setBalance(res.data.balance);
    } catch (err) {
      console.error("Failed to fetch balance", err);
    }
  }, [accountId]);


  useEffect(() => {
    fetchBalance();
  }, [fetchBalance]);


  async function handleSubmit(e) {
    e.preventDefault();
    setMessage("");
    try {
      const token = localStorage.getItem("token");
      await axios.post(
        "/withdraw",
        {
          accountId: parseInt(accountId),
          amount: parseFloat(amount)
        },
        {
          headers: {
            Authorization: `Bearer ${token}`
          }
        }
      );
      setMessage("Withdrawal successful");
      fetchBalance();
      navigate("/dashboard");
    } catch (err) {
      setMessage("Withdrawal failed");
    }
  }


  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Withdraw Funds</Typography>
      <Typography variant="h6" gutterBottom>Your Balance: ${balance}</Typography>
      {message && <Alert severity="info" sx={{ mb: 2 }}>{message}</Alert>}
      <Box component="form" onSubmit={handleSubmit} sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
        <TextField label="Amount" type="number" value={amount} onChange={(e) => setAmount(e.target.value)} required />
        <Button variant="contained" type="submit">Submit Withdrawal</Button>
        <Button variant="outlined" onClick={() => navigate("/dashboard")}>Back to Dashboard</Button>
      </Box>
    </Container>
  );
}


export default Withdraw;